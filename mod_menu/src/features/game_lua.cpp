#include "game_lua.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <MinHook.h>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <deque>
#include <string>

namespace Features::GameLua {

struct lua_State;
using lua_pcall_t       = int(__fastcall*)(lua_State* L, int nargs, int nresults, int errfunc);
using luaL_loadstring_t = int(__fastcall*)(lua_State* L, const char* s);
using lua_settop_t      = void(__fastcall*)(lua_State* L, int idx);
using lua_tolstring_t   = const char*(__fastcall*)(lua_State* L, int idx, size_t* len);

static luaL_loadstring_t s_loadstring = nullptr;
static lua_settop_t      s_settop     = nullptr;
static lua_tolstring_t   s_tolstring  = nullptr;
static lua_pcall_t       o_pcall      = nullptr;
static lua_State*        s_L          = nullptr;

static int s_ScanFound = 0;
static int s_ScanTotal = 5;

// ── Game-thread command queue ───────────────────────────────────────────────
// CRITICAL: MGSV runs its Lua VM on the game thread. We must NOT call into
// s_L from the render thread (the DX11 Present hook). Instead, feature ticks
// (render thread) enqueue Lua here, and we drain the queue from inside the
// pcall hook, which is guaranteed to be running on the game thread with the
// VM in a valid state. This is the same approach IHHook uses.
static CRITICAL_SECTION       s_queueLock;
static bool                   s_queueInit = false;
static std::deque<std::string> s_queue;
static bool                   s_draining  = false;
static constexpr size_t       kMaxQueue   = 128;

static void Enqueue(const char* code) {
    if (!s_queueInit) return;
    EnterCriticalSection(&s_queueLock);
    if (s_queue.size() < kMaxQueue) s_queue.emplace_back(code);
    LeaveCriticalSection(&s_queueLock);
}

// Compile + run one chunk on the CURRENT (game) thread, leaving the Lua stack
// exactly as it was. loadstring pushes 1; a balanced pcall(0,0) pops it on
// success; on error an error object is left which we pop with a relative
// settop(-2) (== lua_pop 1). No absolute stack index needed, so this is safe
// to run while nested inside the game's own pcall (its args stay intact).
static void RunChunkRaw(lua_State* L, const char* code) {
    if (!L || !s_loadstring || !o_pcall) return;
    if (s_loadstring(L, code) != 0) {        // compile error -> error string on top
        if (s_settop) s_settop(L, -2);       // pop it
        return;
    }
    if (o_pcall(L, 0, 0, 0) != 0) {          // runtime error -> error object on top
        if (s_settop) s_settop(L, -2);       // pop it
    }
}

static void DrainQueue(lua_State* L) {
    for (;;) {
        std::string code;
        EnterCriticalSection(&s_queueLock);
        if (s_queue.empty()) { LeaveCriticalSection(&s_queueLock); break; }
        code = std::move(s_queue.front());
        s_queue.pop_front();
        LeaveCriticalSection(&s_queueLock);
        RunChunkRaw(L, code.c_str());
    }
}

static int __fastcall pcall_hook(lua_State* L, int nargs, int nresults, int errfunc) {
    if (L) s_L = L;
    // Drain queued mod commands on the game thread, before the game's own call.
    // Guard against re-entrancy (our own o_pcall below bypasses the hook, but
    // be safe regardless).
    if (L && !s_draining) {
        s_draining = true;
        DrainQueue(L);
        s_draining = false;
    }
    return o_pcall(L, nargs, nresults, errfunc);
}

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();
    s_ScanFound = 0;

    if (!s_queueInit) { InitializeCriticalSection(&s_queueLock); s_queueInit = true; }

    const char* pcallPats[] = {
        "56 48 83 EC 30 44 89 C6 4C 8B 49",
        "56 48 83 EC ?? 44 89 C6 48 8B",
        "48 89 5C 24 ?? 56 48 83 EC 30 44 89 C6",
        nullptr
    };
    for (int i = 0; pcallPats[i]; ++i) {
        uintptr_t addr = Pattern::Scan(pcallPats[i], base, size);
        if (addr) {
            MH_CreateHook(reinterpret_cast<void*>(addr), &pcall_hook,
                          reinterpret_cast<void**>(&o_pcall));
            MH_EnableHook(reinterpret_cast<void*>(addr));
            ++s_ScanFound; break;
        }
    }

    const char* loadPats[] = {
        "48 89 D6 48 89 CF E8 ?? ?? ?? ?? 48 89 F2 44 89 C1",
        "48 89 5C 24 ?? 48 89 6C 24 ?? 56 48 83 EC 20 48 89 CE 48 89 D5",
        nullptr
    };
    for (int i = 0; loadPats[i]; ++i) {
        uintptr_t addr = Pattern::Scan(loadPats[i], base, size);
        if (addr) { s_loadstring = reinterpret_cast<luaL_loadstring_t>(addr); ++s_ScanFound; break; }
    }

    uintptr_t settop = Pattern::Scan("85 D2 78 ?? 48 8B 41 ?? 48 8D 04 D0", base, size);
    if (settop) { s_settop = reinterpret_cast<lua_settop_t>(settop); ++s_ScanFound; }

    uintptr_t tolstr = Pattern::Scan("53 48 83 EC 20 89 D3 48 89 CF E8 ?? ?? ?? ?? 83 78 ?? 04", base, size);
    if (tolstr) { s_tolstring = reinterpret_cast<lua_tolstring_t>(tolstr); ++s_ScanFound; }

    uintptr_t state = Pattern::Scan(
        "48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? BA 01",
        base, size);
    if (state) {
        int32_t off = Memory::Read<int32_t>(state + 3);
        lua_State* candidate = Memory::Read<lua_State*>(state + 7 + off);
        if (candidate) s_L = candidate;
        ++s_ScanFound;
    }
}

bool IsReady() { return s_L && o_pcall && s_loadstring; }

// Queue Lua to run on the game thread (see DrainQueue). Safe to call from the
// render thread / feature ticks.
void RunCode(const char* luaCode) {
    if (!s_L || !s_loadstring || !o_pcall) return;
    Enqueue(luaCode);
}

// Synchronous query that must return a value to the caller this frame. This
// runs on the calling thread; reserve it for read-only probes (e.g. the wave
// HUD's detection check), not for state mutation.
int RunCodeInt(const char* luaCode, int fallback) {
    if (!s_L || !s_loadstring || !o_pcall) return fallback;
    if (s_loadstring(s_L, luaCode) != 0) { if (s_settop) s_settop(s_L, -2); return fallback; }
    if (o_pcall(s_L, 0, 1, 0) != 0)     { if (s_settop) s_settop(s_L, -2); return fallback; }
    int result = fallback;
    if (s_tolstring) { const char* s = s_tolstring(s_L, -1, nullptr); if (s) result = atoi(s); }
    if (s_settop) s_settop(s_L, -2);
    return result;
}

int GetScanFound() { return s_ScanFound; }
int GetScanTotal() { return s_ScanTotal; }

// ── Tick helpers ──────────────────────────────────────────────────────────────

// ══════════════════════════════════════════════════════════════════════════════
// Tick — runs every Present frame; each section tracks "previous" values so
// Lua calls only fire when a config value actually changes, plus periodic
// reassertion timers for things the game constantly overrides.
// ══════════════════════════════════════════════════════════════════════════════

void Tick() {
    if (!IsReady()) return;
    auto& c = Config::Get();
    ULONGLONG now = GetTickCount64();
    char buf[384];

    static ULONGLONG tSlow = 0, tFast = 0;
    bool slow = (now - tSlow > 2500); if (slow) tSlow = now;
    bool fast = (now - tFast > 500);  if (fast) tFast = now;

    // ── God mode (huge HP, reassert every 3s) ─────────────────────────
    static bool pGod = false; static ULONGLONG tGod = 0;
    if (c.godMode != pGod || (c.godMode && now - tGod > 3000)) {
        RunCode(c.godMode
            ? "pcall(function() Player.ResetLifeMaxValue() Player.ChangeLifeMaxValue(50000) end)"
            : "pcall(function() Player.ResetLifeMaxValue() end)");
        pGod = c.godMode; tGod = now;
    }

    // ── Player health scale ───────────────────────────────────────────
    static int pHp = 100;
    if (!c.godMode && c.playerHealthScale != pHp) {
        snprintf(buf, sizeof(buf),
            "pcall(function() Player.ResetLifeMaxValue() "
            "Player.ChangeLifeMaxValue(math.max(10,math.min(50000,%d*50))) end)",
            c.playerHealthScale);
        RunCode(buf); pHp = c.playerHealthScale;
    }

    // ── Infinite ammo / no reload / infinite mags ─────────────────────
    // No direct Lua "set ammo", so we periodically reset all ammo.
    // Works alongside memory patches in player.cpp (whichever succeeds).
    if ((c.infiniteAmmo || c.noReload || c.infiniteMags) && fast) {
        RunCode("pcall(function() "
                "local i=PlayerInfo.GetLocalPlayerIndex() "
                "GameObject.SendCommand({type=\"TppPlayer2\",index=i},{id=\"ResetAmmo\"}) "
                "end)");
    }

    // ── Infinite suppressor ───────────────────────────────────────────
    if (c.infiniteSuppressor && fast) {
        RunCode("pcall(function() "
                "local i=PlayerInfo.GetLocalPlayerIndex() "
                "GameObject.SendCommand({type=\"TppPlayer2\",index=i},{id=\"ResetSuppressor\"}) "
                "end)");
    }

    // ── Infinite throwables / battery ─────────────────────────────────
    if ((c.infiniteThrowables || c.infiniteBattery) && slow) {
        RunCode("pcall(function() "
                "local i=PlayerInfo.GetLocalPlayerIndex() "
                "GameObject.SendCommand({type=\"TppPlayer2\",index=i},{id=\"ResetAmmo\"}) "
                "end)");
    }

    // ── Super speed ───────────────────────────────────────────────────
    static bool pSpd = false; static float pSpdV = 1.0f;
    if (c.superSpeed != pSpd || (c.superSpeed && c.speedMultiplier != pSpdV)) {
        snprintf(buf, sizeof(buf),
            "pcall(function() GameObject.SendCommand("
            "{type=\"TppPlayer2\",index=PlayerInfo.GetLocalPlayerIndex()},"
            "{id=\"SetStandMoveSpeedLimit\",speedRateLimit=%.1f}) end)",
            c.superSpeed ? c.speedMultiplier : -1.0f);
        RunCode(buf); pSpd = c.superSpeed; pSpdV = c.speedMultiplier;
    }

    // ── Noise scale ───────────────────────────────────────────────────
    static int pNoise = 100;
    if (c.noiseScale != pNoise) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.ply_noiseLevelRate=%.2f end end)",
            c.noiseScale / 100.0f);
        RunCode(buf); pNoise = c.noiseScale;
    }

    // ── Silent weapons ────────────────────────────────────────────────
    static bool pSilent = false;
    if (c.silentWeapons != pSilent) {
        RunCode(c.silentWeapons
            ? "pcall(function() if vars then vars.ply_isNoWeaponNoise=true end end)"
            : "pcall(function() if vars then vars.ply_isNoWeaponNoise=false end end)");
        pSilent = c.silentWeapons;
    }

    // ── Unlimited stamina ─────────────────────────────────────────────
    if (c.unlimitedStamina && fast) {
        RunCode("pcall(function() "
                "local i=PlayerInfo.GetLocalPlayerIndex() "
                "GameObject.SendCommand({type=\"TppPlayer2\",index=i},{id=\"ResetStamina\"}) "
                "end)");
    }

    // ── No fall damage (Lua approach: keep HP topped up) ──────────────
    // Memory patch in player.cpp is primary; this is a backup
    if (c.noFallDamage && !c.godMode && fast) {
        RunCode("pcall(function() Player.ChangeLifeMaxValue(0) end)");
    }

    // ── Hero/Demon controls ───────────────────────────────────────────
    static bool pNoSubHero = false;
    if (c.dontSubtractHero != pNoSubHero) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.heroSubtractDisable=%s end end)",
            c.dontSubtractHero ? "true" : "false");
        RunCode(buf); pNoSubHero = c.dontSubtractHero;
    }

    static bool pNoAddOgre = false;
    if (c.dontAddOgre != pNoAddOgre) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ogreAddDisable=%s end end)",
            c.dontAddOgre ? "true" : "false");
        RunCode(buf); pNoAddOgre = c.dontAddOgre;
    }

    // ── Appearance ────────────────────────────────────────────────────
    // NOTE: appearance vars take effect on the next player (re)load, not
    // instantly. Field names verified against MGSV/IH source:
    // vars.playerType / playerPartsType / playerCamoType / playerHandType /
    // playerFaceEquipId / playerFaceId.
    static int pType = -1;
    if (c.playerType != pType) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerType=%d end)", c.playerType);
        RunCode(buf); pType = c.playerType;
    }

    static int pParts = -1;
    if (c.playerParts != pParts) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerPartsType=%d end)", c.playerParts);
        RunCode(buf); pParts = c.playerParts;
    }

    static int pCamo = -1;
    if (c.playerCamo != pCamo) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerCamoType=%d end)", c.playerCamo);
        RunCode(buf); pCamo = c.playerCamo;
    }

    static int pHeadgear = -1;
    if (c.headgear != pHeadgear) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerFaceEquipId=%d end)", c.headgear);
        RunCode(buf); pHeadgear = c.headgear;
    }

    static int pHand = -1;
    if (c.handType != pHand) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerHandType=%d end)", c.handType);
        RunCode(buf); pHand = c.handType;
    }

    static int pGender = -1;
    if (c.avatarGender != pGender) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.avatarGender=%d end)", c.avatarGender);
        RunCode(buf); pGender = c.avatarGender;
    }

    static int pFovaFace = -1;
    if (c.fovaFace != pFovaFace) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerFaceId=%d end)", c.fovaFace);
        RunCode(buf); pFovaFace = c.fovaFace;
    }

    static int pFovaBody = -1;
    if (c.fovaBody != pFovaBody) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerBodyId=%d end)", c.fovaBody);
        RunCode(buf); pFovaBody = c.fovaBody;
    }

    // ── Slow motion (HighSpeedCamera) ─────────────────────────────────
    static bool pSlow = false; static float pTS = 1.0f;
    if (c.slowMotion != pSlow || (c.slowMotion && c.timeScale != pTS)) {
        if (c.slowMotion) {
            snprintf(buf, sizeof(buf),
                "pcall(function() HighSpeedCamera.RequestEvent{"
                "continueTime=999999,worldTimeScale=%.4f,"
                "playerTimeScale=1.0,noDustEffect=false} end)", c.timeScale);
            RunCode(buf);
        } else {
            RunCode("pcall(function() HighSpeedCamera.RequestToCancel() end)");
        }
        pSlow = c.slowMotion; pTS = c.timeScale;
    }

    // ── Time of day ───────────────────────────────────────────────────
    static bool pOvTm = false; static float pTOD = 12.0f;
    if (c.overrideTime != pOvTm || (c.overrideTime && c.timeOfDay != pTOD)) {
        if (c.overrideTime) {
            int h = (int)c.timeOfDay, m = (int)((c.timeOfDay - h) * 60.0f);
            snprintf(buf, sizeof(buf),
                "pcall(function() TppClock.SetTime(\"%02d:%02d:00\") TppClock.Start() end)", h, m);
            RunCode(buf);
        }
        pOvTm = c.overrideTime; pTOD = c.timeOfDay;
    }

    // ── Clock time scale ──────────────────────────────────────────────
    static int pClkScale = 1;
    if (c.clockTimeScale != pClkScale) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppClock.SetTimeScale then TppClock.SetTimeScale(%d) end end)",
            c.clockTimeScale);
        RunCode(buf); pClkScale = c.clockTimeScale;
    }

    // ── Weather ───────────────────────────────────────────────────────
    static int pWeather = -1;
    if (c.weatherOverride) {
        if (c.weather != pWeather && c.weather > 0 && c.weather < 7) {
            snprintf(buf, sizeof(buf),
                "pcall(function() TppWeather.ForceRequestWeather(%d,2.0) end)", c.weather);
            RunCode(buf); pWeather = c.weather;
        }
    } else if (pWeather > 0) {
        RunCode("pcall(function() TppWeather.CancelForceRequestWeather() end)");
        pWeather = 0;
    }

    // ── Helicopter ────────────────────────────────────────────────────
    static bool pInvH = false;
    if (c.invincibleHeli != pInvH) {
        snprintf(buf, sizeof(buf),
            "pcall(function() TppHelicopter.SetInvincible(%s) end)",
            c.invincibleHeli ? "true" : "false");
        RunCode(buf); pInvH = c.invincibleHeli;
    }

    static int pLight = -1;
    if (c.searchLight != pLight) {
        if (c.searchLight == 1)
            RunCode("pcall(function() TppHelicopter.SetSearchLight(false) end)");
        else if (c.searchLight == 2)
            RunCode("pcall(function() TppHelicopter.SetSearchLight(true) end)");
        pLight = c.searchLight;
    }

    static bool pPullOut = false;
    if (c.disablePullOut != pPullOut) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppHelicopter.SetForcedStay then TppHelicopter.SetForcedStay(%s) end end)",
            c.disablePullOut ? "true" : "false");
        RunCode(buf); pPullOut = c.disablePullOut;
    }

    static bool pHeliStealth = false;
    if (c.heliStealth != pHeliStealth) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppHelicopter.SetStealth then TppHelicopter.SetStealth(%s) end end)",
            c.heliStealth ? "true" : "false");
        RunCode(buf); pHeliStealth = c.heliStealth;
    }

    // ── Enemy phase ───────────────────────────────────────────────────
    static int pPhase = 0;
    if (c.enemyPhase != pPhase) {
        if (c.enemyPhase >= 1 && c.enemyPhase <= 4) {
            const char* phases[] = { "", "SNEAK", "CAUTION", "EVASION", "ALERT" };
            snprintf(buf, sizeof(buf),
                "pcall(function() "
                "if TppMission and TppMission.SetPhase then TppMission.SetPhase(\"%s\") end "
                "end)", phases[c.enemyPhase]);
            RunCode(buf);
        }
        pPhase = c.enemyPhase;
    }

    // ── Enemy detection scales ────────────────────────────────────────
    static int pSight = 100;
    if (c.soldierSightScale != pSight) {
        snprintf(buf, sizeof(buf),
            "pcall(function() "
            "if gvars then gvars.soldierSightDistRate=%.2f end "
            "if TppSoldier2 and TppSoldier2.SetSightParam then "
            "TppSoldier2.SetSightParam{sightDistRate=%.2f} end end)",
            c.soldierSightScale / 100.0f, c.soldierSightScale / 100.0f);
        RunCode(buf); pSight = c.soldierSightScale;
    }

    static int pHear = 100;
    if (c.soldierHearingScale != pHear) {
        snprintf(buf, sizeof(buf),
            "pcall(function() "
            "if gvars then gvars.soldierHearingRate=%.2f end "
            "if TppSoldier2 and TppSoldier2.SetHearingParam then "
            "TppSoldier2.SetHearingParam{hearingRate=%.2f} end end)",
            c.soldierHearingScale / 100.0f, c.soldierHearingScale / 100.0f);
        RunCode(buf); pHear = c.soldierHearingScale;
    }

    // ── Enemy prep (revenge system) ───────────────────────────────────
    // These control what counter-gear enemies bring
    static int pPrepHash = -1;
    int prepHash = c.prepSniper + c.prepMissile*3 + c.prepMG*7 + c.prepShotgun*11 +
                   c.prepArmor*13 + c.prepShield*17 + c.prepHelmet*19 + c.prepNVG*23 +
                   c.prepGasMask*29;
    if (prepHash != pPrepHash) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppRevenge then "
            "local s=TppRevenge.SetOspreyCombatGimmickCount or function() end "
            "s('SNIPER',%d) s('MISSILE',%d) s('MG',%d) s('SHOTGUN',%d) "
            "s('ARMOR',%d) s('SHIELD',%d) s('HELMET',%d) s('NVG',%d) s('GAS_MASK',%d) "
            "end end)",
            c.prepSniper, c.prepMissile, c.prepMG, c.prepShotgun,
            c.prepArmor, c.prepShield, c.prepHelmet, c.prepNVG, c.prepGasMask);
        RunCode(buf); pPrepHash = prepHash;
    }

    // ── Enemy behavior ────────────────────────────────────────────────
    static bool pDisRadio = false;
    if (c.disableRadioCall != pDisRadio) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ene_disableRadioCall=%s end end)",
            c.disableRadioCall ? "true" : "false");
        RunCode(buf); pDisRadio = c.disableRadioCall;
    }

    static bool pNoAlert = false;
    if (c.noAlertPropagation != pNoAlert) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ene_noAlertPropagation=%s end end)",
            c.noAlertPropagation ? "true" : "false");
        RunCode(buf); pNoAlert = c.noAlertPropagation;
    }

    // ── Mission settings ──────────────────────────────────────────────
    static bool pGOD = false;
    if (c.gameOverOnDiscovery != pGOD) {
        if (c.gameOverOnDiscovery)
            RunCode("pcall(function() TppMission.RegistDiscoveryGameOver() end)");
        pGOD = c.gameOverOnDiscovery;
    }

    static bool pNoGameOver = false;
    if (c.disableGameOver != pNoGameOver) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if mvars then mvars.mis_isDisableGameOver=%s end end)",
            c.disableGameOver ? "true" : "false");
        RunCode(buf); pNoGameOver = c.disableGameOver;
    }

    // Verified mechanism: disabling reflex-mode is done via the player action
    // flag (PlayerDisableAction.REFLEXMODE), the same way mission scripts do
    // it. Reassert on the slow timer because missions reset the flag.
    static bool pNoReflex = false;
    if (c.noReflex != pNoReflex || (c.noReflex && slow)) {
        RunCode(c.noReflex
            ? "pcall(function() vars.playerDisableActionFlag = PlayerDisableAction.REFLEXMODE end)"
            : "pcall(function() vars.playerDisableActionFlag = PlayerDisableAction.NONE end)");
        pNoReflex = c.noReflex;
    }

    static bool pNoMark = false;
    if (c.noMarking != pNoMark) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.mis_noMarking=%s end end)",
            c.noMarking ? "true" : "false");
        RunCode(buf); pNoMark = c.noMarking;
    }

    static bool pSubsist = false;
    if (c.setSubsistence != pSubsist) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if mvars then mvars.mis_isSubsistence=%s end end)",
            c.setSubsistence ? "true" : "false");
        RunCode(buf); pSubsist = c.setSubsistence;
    }

    static bool pForceNight = false;
    if (c.forceNight != pForceNight) {
        if (c.forceNight)
            RunCode("pcall(function() TppClock.SetTime(\"02:00:00\") TppClock.Stop() end)");
        else if (!c.overrideTime)
            RunCode("pcall(function() TppClock.Start() end)");
        pForceNight = c.forceNight;
    }

    static bool pForceDay = false;
    if (c.forceDay != pForceDay) {
        if (c.forceDay)
            RunCode("pcall(function() TppClock.SetTime(\"12:00:00\") TppClock.Stop() end)");
        else if (!c.overrideTime)
            RunCode("pcall(function() TppClock.Start() end)");
        pForceDay = c.forceDay;
    }

    // ── Buddy equipment ───────────────────────────────────────────────
    static int pQW = -1;
    if (c.quietWeapon != pQW) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.quietWeaponId=%d end end)", c.quietWeapon);
        RunCode(buf); pQW = c.quietWeapon;
    }

    static int pDE = -1;
    if (c.ddogEquip != pDE) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.ddogEquipId=%d end end)", c.ddogEquip);
        RunCode(buf); pDE = c.ddogEquip;
    }

    static int pHE = -1;
    if (c.dhorseEquip != pHE) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.dhorseEquipId=%d end end)", c.dhorseEquip);
        RunCode(buf); pHE = c.dhorseEquip;
    }

    static int pWE = -1;
    if (c.dwalkerEquip != pWE) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.dwalkerEquipId=%d end end)", c.dwalkerEquip);
        RunCode(buf); pWE = c.dwalkerEquip;
    }

    // ── Fulton ────────────────────────────────────────────────────────
    if (c.infiniteFulton && slow)
        RunCode("pcall(function() if vars then vars.fultonCount=999 end end)");

    static bool pFulAll = false;
    if (c.fultonEverything != pFulAll) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then "
            "gvars.ful_isEnableFultonAll=%s "
            "gvars.ful_isEnableFultonVehicle=%s "
            "gvars.ful_isEnableFultonContainer=%s "
            "end end)",
            c.fultonEverything ? "true" : "false",
            c.fultonEverything ? "true" : "false",
            c.fultonEverything ? "true" : "false");
        RunCode(buf); pFulAll = c.fultonEverything;
    }

    static bool pFulVeh = false;
    if (c.fultonVehicles != pFulVeh && !c.fultonEverything) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ful_isEnableFultonVehicle=%s end end)",
            c.fultonVehicles ? "true" : "false");
        RunCode(buf); pFulVeh = c.fultonVehicles;
    }

    static bool pFulCont = false;
    if (c.fultonContainers != pFulCont && !c.fultonEverything) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ful_isEnableFultonContainer=%s end end)",
            c.fultonContainers ? "true" : "false");
        RunCode(buf); pFulCont = c.fultonContainers;
    }

    // ── Resources (GMP / Heroism / Demon) ─────────────────────────────
    static ULONGLONG tRes = 0;
    static int pGmp = -1, pHero = -1, pDemon = -1;
    bool rTimer = (now - tRes > 2000);

    if (c.lockGMP && (c.gmp != pGmp || rTimer)) {
        snprintf(buf, sizeof(buf),
            "pcall(function() TppMotherBaseManagement.SetGmp{gmp=%d} end)", c.gmp);
        RunCode(buf); pGmp = c.gmp;
    }
    if (c.lockHeroism && (c.heroism != pHero || rTimer)) {
        snprintf(buf, sizeof(buf),
            "pcall(function() TppMotherBaseManagement.SetHeroicPoint{heroicPoint=%d} end)",
            c.heroism);
        RunCode(buf); pHero = c.heroism;
    }
    if (c.lockDemonPoints && (c.demonPoints != pDemon || rTimer)) {
        snprintf(buf, sizeof(buf),
            "pcall(function() TppMotherBaseManagement.SetOgrePoint{ogrePoint=%d} end)",
            c.demonPoints);
        RunCode(buf); pDemon = c.demonPoints;
    }
    if (rTimer) tRes = now;

    // ── Cutscenes ─────────────────────────────────────────────────────
    static bool pSoldierDemo = false;
    if (c.useSoldierForDemos != pSoldierDemo) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.demoIsUseSoldier=%s end end)",
            c.useSoldierForDemos ? "true" : "false");
        RunCode(buf); pSoldierDemo = c.useSoldierForDemos;
    }

    // ── Mother Base ───────────────────────────────────────────────────
    static bool pMBOcelot = false;
    if (c.mbEnableOcelot != pMBOcelot) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.mb_isEnableOcelot=%s end end)",
            c.mbEnableOcelot ? "true" : "false");
        RunCode(buf); pMBOcelot = c.mbEnableOcelot;
    }

    static bool pMBPuppy = false;
    if (c.mbEnablePuppy != pMBPuppy) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.mb_isEnablePuppy=%s end end)",
            c.mbEnablePuppy ? "true" : "false");
        RunCode(buf); pMBPuppy = c.mbEnablePuppy;
    }

    static bool pMBBuddies = false;
    if (c.mbEnableBuddies != pMBBuddies) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.mb_isEnableBuddies=%s end end)",
            c.mbEnableBuddies ? "true" : "false");
        RunCode(buf); pMBBuddies = c.mbEnableBuddies;
    }

    // ── Vehicle ───────────────────────────────────────────────────────
    static bool pVehGod = false;
    if (c.vehicleGodMode != pVehGod) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if vars then vars.veh_isInvincible=%s end end)",
            c.vehicleGodMode ? "true" : "false");
        RunCode(buf); pVehGod = c.vehicleGodMode;
    }

    // Infinite vehicle ammo
    if (c.infiniteVehicleAmmo && slow) {
        RunCode("pcall(function() "
                "if vars then vars.veh_ammoCount=999 end end)");
    }

    // ── Skulls in free roam ───────────────────────────────────────────
    static bool pSkullsFR = false;
    if (c.skullsInFreeRoam != pSkullsFR) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.skl_isEnableSkulls=%s end end)",
            c.skullsInFreeRoam ? "true" : "false");
        RunCode(buf); pSkullsFR = c.skullsInFreeRoam;
    }

    // ── Progression / Unlocks ─────────────────────────────────────────
    // One-shot unlock commands (fire once when toggled on)
    static bool pUnlkW = false;
    if (c.unlockAllWeapons && !pUnlkW) {
        RunCode("pcall(function() "
                "if TppMotherBaseManagement.UnlockAllWeaponBlueprint then "
                "TppMotherBaseManagement.UnlockAllWeaponBlueprint() end end)");
        pUnlkW = true;
    } else if (!c.unlockAllWeapons) pUnlkW = false;

    static bool pUnlkI = false;
    if (c.unlockAllItems && !pUnlkI) {
        RunCode("pcall(function() "
                "if TppMotherBaseManagement.UnlockAllItemBlueprint then "
                "TppMotherBaseManagement.UnlockAllItemBlueprint() end end)");
        pUnlkI = true;
    } else if (!c.unlockAllItems) pUnlkI = false;

    static bool pUnlkM = false;
    if (c.unlockAllMissions && !pUnlkM) {
        RunCode("pcall(function() "
                "if TppMission and TppMission.UnlockAllMission then "
                "TppMission.UnlockAllMission() end end)");
        pUnlkM = true;
    } else if (!c.unlockAllMissions) pUnlkM = false;

    static bool pUnlkS = false;
    if (c.unlockAllSideOps && !pUnlkS) {
        RunCode("pcall(function() "
                "if TppQuest and TppQuest.UnlockAllQuest then "
                "TppQuest.UnlockAllQuest() end end)");
        pUnlkS = true;
    } else if (!c.unlockAllSideOps) pUnlkS = false;

    // ── Cassette player ───────────────────────────────────────────────
    static int pCass = 0;
    if (c.cassetteTrack != pCass && c.cassetteTrack > 0) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppUiCommand and TppUiCommand.PlayCassette then "
            "TppUiCommand.PlayCassette(%d) end end)", c.cassetteTrack);
        RunCode(buf); pCass = c.cassetteTrack;
    }

    // ── No enemy AI (Lua fallback) ────────────────────────────────────
    // Memory patch in world.cpp is primary; this is a backup via gvars
    static bool pNoAI = false;
    if (c.noEnemyAI != pNoAI) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if gvars then gvars.ene_isDisableAI=%s end end)",
            c.noEnemyAI ? "true" : "false");
        RunCode(buf); pNoAI = c.noEnemyAI;
    }

    // ── Skip cutscenes ────────────────────────────────────────────────
    if (c.skipAllCutscenes && fast) {
        RunCode("pcall(function() if TppDemo and TppDemo.Skip then TppDemo.Skip() end end)");
    }

    // ── Side ops force quest ──────────────────────────────────────────
    static int pForceQuest = 0;
    if (c.forceQuestNumber != pForceQuest && c.forceQuestNumber > 0) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppQuest and TppQuest.ForceStartQuest then "
            "TppQuest.ForceStartQuest(%d) end end)", c.forceQuestNumber);
        RunCode(buf); pForceQuest = c.forceQuestNumber;
    }

    // ── Night vision (Lua brightness boost) ───────────────────────────
    // Memory approach in visuals.cpp is primary; this boosts via GrTools
    static bool pNV = false;
    if (c.nightVision != pNV) {
        if (c.nightVision) {
            snprintf(buf, sizeof(buf),
                "pcall(function() if GrTools and GrTools.SetBrightness then "
                "GrTools.SetBrightness(%.1f) end end)", c.nightVisionStr);
            RunCode(buf);
        } else {
            RunCode("pcall(function() if GrTools and GrTools.SetBrightness then "
                    "GrTools.SetBrightness(1.0) end end)");
        }
        pNV = c.nightVision;
    }
}

// ── Diagnostics ───────────────────────────────────────────────────────────────

void RunDiagnostics() {
    if (!IsReady()) {
        RunCode("pcall(function() TppUiCommand.AnnounceLogView('IH: Lua NOT ready') end)");
        return;
    }
    RunCode(
        "pcall(function() TppUiCommand.AnnounceLogView('IH: Lua bridge CONNECTED') end) "
        "pcall(function() TppUiCommand.AnnounceLogView('vars = '..tostring(type(vars))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('mvars = '..tostring(type(mvars))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('gvars = '..tostring(type(gvars))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('Player = '..tostring(type(Player))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('PlayerInfo = '..tostring(type(PlayerInfo))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('GameObject = '..tostring(type(GameObject))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppWeather = '..tostring(type(TppWeather))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppClock = '..tostring(type(TppClock))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppMBMgmt = '..tostring(type(TppMotherBaseManagement))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppHelicopter = '..tostring(type(TppHelicopter))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppMission = '..tostring(type(TppMission))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('HighSpeedCam = '..tostring(type(HighSpeedCamera))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppSoldier2 = '..tostring(type(TppSoldier2))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppRevenge = '..tostring(type(TppRevenge))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppQuest = '..tostring(type(TppQuest))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppDemo = '..tostring(type(TppDemo))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('GrTools = '..tostring(type(GrTools))) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('TppUiCommand = '..tostring(type(TppUiCommand))) end)"
    );
}

} // namespace Features::GameLua
