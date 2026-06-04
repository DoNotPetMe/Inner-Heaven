#include "game_lua.h"
#include "lua_console.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <MinHook.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <deque>
#include <string>

namespace Features::GameLua {

// ── Lua C API ───────────────────────────────────────────────────────────────
// Signatures, patterns and the overall approach are ported from IHHook
// (github.com/TinManTex/IHHook, by TinManTex) which maintains these for current
// MGSV:TPP builds. Two things this fixes vs. the old hand-rolled bridge:
//   1. The game does NOT export luaL_loadstring (IHHook marks it "USING_CODE").
//      Compilation goes through luaL_loadbuffer instead.
//   2. lua_State is captured from the lua_pcall detour (IHHook: usingDetour),
//      not by guessing a global pointer.
struct lua_State;
using lua_pcall_t       = int(__fastcall*)(lua_State* L, int nargs, int nresults, int errfunc);
using luaL_loadbuffer_t = int(__fastcall*)(lua_State* L, const char* buff, size_t sz, const char* name);
using lua_settop_t      = void(__fastcall*)(lua_State* L, int idx);
using lua_tolstring_t   = const char*(__fastcall*)(lua_State* L, int idx, size_t* len);

static luaL_loadbuffer_t s_loadbuffer = nullptr;
static lua_settop_t      s_settop     = nullptr;
static lua_tolstring_t   s_tolstring  = nullptr;
static lua_pcall_t       o_pcall      = nullptr;
static lua_State*        s_L          = nullptr;

static int s_ScanFound = 0;
static int s_ScanTotal = 4;  // pcall, loadbuffer, settop, tolstring

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
    if (!L || !s_loadbuffer || !o_pcall) return;
    // luaL_loadbuffer(L, buff, size, chunkname). Pushes the compiled chunk on
    // success, or an error string on failure.
    if (s_loadbuffer(L, code, strlen(code), "IH") != 0) {  // compile error
        if (s_settop) s_settop(L, -2);       // pop it
        return;
    }
    if (o_pcall(L, 0, 0, 0) != 0) {          // runtime error -> error object on top
        if (s_settop) s_settop(L, -2);       // pop it
    }
}

// Probe result buffer — written on game thread, read on render thread, guarded
// by s_queueLock. This avoids ANY synchronous Lua calls from the render thread.
static bool        s_ProbePending     = false;
static std::string s_ProbeResultBuf;
static bool        s_ProbeResultReady = false;

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

    // After draining, check if the probe left a result. This runs on the GAME
    // THREAD so it's safe to read Lua here.
    if (s_ProbePending && !s_ProbeResultReady && L && s_loadbuffer && s_tolstring && s_settop) {
        const char* check = "return IH_PROBE_RESULT or ''";
        if (s_loadbuffer(L, check, strlen(check), "IH") == 0) {
            if (o_pcall(L, 0, 1, 0) == 0) {
                const char* s = s_tolstring(L, -1, nullptr);
                if (s && s[0]) {
                    EnterCriticalSection(&s_queueLock);
                    s_ProbeResultBuf = s;
                    s_ProbeResultReady = true;
                    LeaveCriticalSection(&s_queueLock);
                    // Clear the global
                    RunChunkRaw(L, "IH_PROBE_RESULT = nil");
                }
                s_settop(L, -2);
            } else {
                if (s_settop) s_settop(L, -2);
            }
        } else {
            if (s_settop) s_settop(L, -2);
        }
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

    // Verified patterns from IHHook (mgsvtpp_patterns.h / lua_Signatures.h).
    // These are maintained against current MGSV builds and are version-
    // independent, unlike hard-coded addresses.

    // lua_pcall — hook it so we (a) capture lua_State and (b) drain our queue
    // on the game thread.
    uintptr_t pcall = Pattern::Scan("48 89 5C 24 ? 57 48 83 EC 40 44 89 C7", base, size);
    if (pcall) {
        if (MH_CreateHook(reinterpret_cast<void*>(pcall), &pcall_hook,
                          reinterpret_cast<void**>(&o_pcall)) == MH_OK &&
            MH_EnableHook(reinterpret_cast<void*>(pcall)) == MH_OK) {
            ++s_ScanFound;
        }
    }

    // luaL_loadbuffer — the game has no luaL_loadstring; compile via this.
    uintptr_t loadbuf = Pattern::Scan("48 83 EC 38 48 89 54 24 ? 4C 89 44 24 ?", base, size);
    if (loadbuf) { s_loadbuffer = reinterpret_cast<luaL_loadbuffer_t>(loadbuf); ++s_ScanFound; }

    // lua_settop — for stack cleanup.
    uintptr_t settop = Pattern::Scan("85 D2 78 ? 4C 63 ? 48 8B", base, size);
    if (settop) { s_settop = reinterpret_cast<lua_settop_t>(settop); ++s_ScanFound; }

    // lua_tolstring — for reading return values (RunCodeInt).
    uintptr_t tolstr = Pattern::Scan(
        "48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 4C 89 ? 89 D6 48 89 ? E8 ? ? ? ? "
        "49 89 ? 83 78 08 ? 74 ? 48 89 ? 48 89 ? E8 ? ? ? ? 85 C0 75 ? 48 85",
        base, size);
    if (tolstr) { s_tolstring = reinterpret_cast<lua_tolstring_t>(tolstr); ++s_ScanFound; }

    // lua_State itself is captured live in pcall_hook (IHHook's usingDetour
    // approach) — far more reliable than guessing a global pointer.
}

bool IsReady() { return s_L && o_pcall && s_loadbuffer; }

// Queue Lua to run on the game thread (see DrainQueue). Safe to call from the
// render thread / feature ticks.
void RunCode(const char* luaCode) {
    if (!s_L || !s_loadbuffer || !o_pcall) return;
    Enqueue(luaCode);
}

// Synchronous query that must return a value to the caller this frame. This
// runs on the calling thread; reserve it for read-only probes (e.g. the wave
// HUD's detection check), not for state mutation.
int RunCodeInt(const char* luaCode, int fallback) {
    if (!s_L || !s_loadbuffer || !o_pcall) return fallback;
    if (s_loadbuffer(s_L, luaCode, strlen(luaCode), "IH") != 0) { if (s_settop) s_settop(s_L, -2); return fallback; }
    if (o_pcall(s_L, 0, 1, 0) != 0)     { if (s_settop) s_settop(s_L, -2); return fallback; }
    int result = fallback;
    if (s_tolstring) { const char* s = s_tolstring(s_L, -1, nullptr); if (s) result = atoi(s); }
    if (s_settop) s_settop(s_L, -2);
    return result;
}

// Same as RunCodeInt but preserves fractional precision (for world coordinates).
float RunCodeFloat(const char* luaCode, float fallback) {
    if (!s_L || !s_loadbuffer || !o_pcall) return fallback;
    if (s_loadbuffer(s_L, luaCode, strlen(luaCode), "IH") != 0) { if (s_settop) s_settop(s_L, -2); return fallback; }
    if (o_pcall(s_L, 0, 1, 0) != 0)     { if (s_settop) s_settop(s_L, -2); return fallback; }
    float result = fallback;
    if (s_tolstring) { const char* s = s_tolstring(s_L, -1, nullptr); if (s) result = (float)atof(s); }
    if (s_settop) s_settop(s_L, -2);
    return result;
}

std::string RunCodeStr(const char* luaCode, const char* fallback) {
    std::string fb = fallback ? fallback : "";
    if (!s_L || !s_loadbuffer || !o_pcall) return fb;
    if (s_loadbuffer(s_L, luaCode, strlen(luaCode), "IH") != 0) { if (s_settop) s_settop(s_L, -2); return fb; }
    if (o_pcall(s_L, 0, 1, 0) != 0)     { if (s_settop) s_settop(s_L, -2); return fb; }
    std::string result = fb;
    if (s_tolstring) { const char* s = s_tolstring(s_L, -1, nullptr); if (s) result = s; }  // copy before pop
    if (s_settop) s_settop(s_L, -2);
    return result;
}

int GetScanFound() { return s_ScanFound; }
int GetScanTotal() { return s_ScanTotal; }

static void PollProbeResult();   // defined after Tick

// ── Tick helpers ──────────────────────────────────────────────────────────────

// ══════════════════════════════════════════════════════════════════════════════
// Tick — runs every Present frame.
//
// VERIFIED vs UNVERIFIED: Every Lua call below has been cross-referenced
// against TinManTex/mgsv-deminified-lua (the decompiled game scripts) and
// TinManTex/InfiniteHeaven. Only calls using REAL game APIs are active.
// Features that used fabricated field names have been removed — they wrote to
// non-existent gvars/mvars/vars fields and silently did nothing. Those
// features need either (a) AOB memory patches or (b) Infinite Heaven loaded.
// ══════════════════════════════════════════════════════════════════════════════

void Tick() {
    if (!IsReady()) return;
    PollProbeResult();
    auto& c = Config::Get();
    ULONGLONG now = GetTickCount64();
    char buf[512];

    static ULONGLONG tSlow = 0, tFast = 0;
    bool slow = (now - tSlow > 2500); if (slow) tSlow = now;
    bool fast = (now - tFast > 500);  if (fast) tFast = now;

    // ══════════════════════════════════════════════════════════════════
    //  SECTION 1: VERIFIED WORKING — real game Lua APIs
    // ══════════════════════════════════════════════════════════════════

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

    // ── Unlimited stamina ─────────────────────────────────────────────
    if (c.unlimitedStamina && fast) {
        RunCode("pcall(function() "
                "local i=PlayerInfo.GetLocalPlayerIndex() "
                "GameObject.SendCommand({type=\"TppPlayer2\",index=i},{id=\"ResetStamina\"}) "
                "end)");
    }

    // ── No fall damage (Lua approach: keep HP topped up) ──────────────
    if (c.noFallDamage && !c.godMode && fast) {
        RunCode("pcall(function() Player.ChangeLifeMaxValue(0) end)");
    }

    // ── Appearance ────────────────────────────────────────────────────
    // Verified field names from TppPlayer.lua (deminified source):
    //   vars.playerType, vars.playerPartsType, vars.playerCamoType,
    //   vars.playerFaceEquipId, vars.handEquip, vars.playerFaceId
    // Changes take effect on the next player (re)load, not instantly.
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

    // FIX: was vars.playerHandType (wrong). Correct field: vars.handEquip
    static int pHand = -1;
    if (c.handType != pHand) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.handEquip=%d end)", c.handType);
        RunCode(buf); pHand = c.handType;
    }

    static int pFovaFace = -1;
    if (c.fovaFace != pFovaFace) {
        snprintf(buf, sizeof(buf), "pcall(function() vars.playerFaceId=%d end)", c.fovaFace);
        RunCode(buf); pFovaFace = c.fovaFace;
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

    // ── Hero/Demon subtraction control ────────────────────────────────
    // No native gvar for this (IH uses its own Ivars hook). Best we can
    // do: when "don't subtract hero" is on, periodically reassert a high
    // heroism value; when "don't add ogre" is on, reassert 0 ogre.
    if (c.dontSubtractHero && slow) {
        RunCode("pcall(function() "
                "TppMotherBaseManagement.SetHeroicPoint{heroicPoint=999999} end)");
    }
    if (c.dontAddOgre && slow) {
        RunCode("pcall(function() "
                "TppMotherBaseManagement.SetOgrePoint{ogrePoint=0} end)");
    }

    // ── No reflex ─────────────────────────────────────────────────────
    static bool pNoReflex = false;
    if (c.noReflex != pNoReflex || (c.noReflex && slow)) {
        RunCode(c.noReflex
            ? "pcall(function() vars.playerDisableActionFlag = PlayerDisableAction.REFLEXMODE end)"
            : "pcall(function() vars.playerDisableActionFlag = PlayerDisableAction.NONE end)");
        pNoReflex = c.noReflex;
    }

    // ── Game over on discovery ────────────────────────────────────────
    static bool pGOD = false;
    if (c.gameOverOnDiscovery != pGOD) {
        if (c.gameOverOnDiscovery)
            RunCode("pcall(function() TppMission.RegistDiscoveryGameOver() end)");
        pGOD = c.gameOverOnDiscovery;
    }

    // ── Force night / day ─────────────────────────────────────────────
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

    // ── Enemy prep (revenge system) ───────────────────────────────────
    // Verified: TppRevenge.SetRevengePoint(type, points) controls what
    // counter-gear enemies bring. REVENGE_TYPE enum from deminified source:
    //   STEALTH=0, NIGHT_S=1, COMBAT=2, NIGHT_C=3, LONG_RANGE=4,
    //   VEHICLE=5, HEAD_SHOT=6, TRANQ=7, FULTON=8, SMOKE=9
    // Higher points = more of that equipment. Map our prep sliders to
    // the closest revenge types.
    static int pPrepHash = -1;
    int prepHash = c.prepSniper + c.prepMissile*3 + c.prepMG*7 + c.prepShotgun*11 +
                   c.prepArmor*13 + c.prepShield*17 + c.prepHelmet*19 + c.prepNVG*23 +
                   c.prepGasMask*29;
    if (prepHash != pPrepHash) {
        // Scale 0-100 sliders to 0-1000 revenge points (the game uses ~200-800 range)
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppRevenge and TppRevenge.SetRevengePoint then "
            "local s=TppRevenge.SetRevengePoint "
            "s(4,%d) "  // LONG_RANGE → snipers
            "s(5,%d) "  // VEHICLE → missiles/heavy
            "s(2,%d) "  // COMBAT → MG/shotgun/armor/shield
            "s(6,%d) "  // HEAD_SHOT → helmets
            "s(1,%d) "  // NIGHT_S → NVG
            "s(0,%d) "  // STEALTH → cameras/mines/gas masks
            "end end)",
            c.prepSniper * 10,
            c.prepMissile * 10,
            (c.prepMG + c.prepShotgun + c.prepArmor + c.prepShield) * 3,
            c.prepHelmet * 10,
            c.prepNVG * 10,
            (c.prepGasMask + c.prepDecoy + c.prepMine + c.prepCamera) * 3);
        RunCode(buf); pPrepHash = prepHash;
    }

    // ── Skip cutscenes ────────────────────────────────────────────────
    // FIX: TppDemo.Skip doesn't exist. Real function: DemoDaemon.SkipAll()
    if (c.skipAllCutscenes && fast) {
        RunCode("pcall(function() if DemoDaemon and DemoDaemon.SkipAll then "
                "DemoDaemon.SkipAll() end end)");
    }

    // ── Clock time scale ──────────────────────────────────────────────
    static int pClkScale = 1;
    if (c.clockTimeScale != pClkScale) {
        snprintf(buf, sizeof(buf),
            "pcall(function() if TppClock and TppClock.SetTimeScale then "
            "TppClock.SetTimeScale(%d) end end)",
            c.clockTimeScale);
        RunCode(buf); pClkScale = c.clockTimeScale;
    }

    // ── Fulton: guaranteed success ────────────────────────────────────
    // No gvars.ful_* fields exist in the game. Fulton success is item-level
    // based. We can force 100% success via the verified mvars field.
    // Memory patch in world.cpp handles infinite fulton count.
    static bool pFulAll = false;
    if (c.fultonEverything != pFulAll) {
        RunCode(c.fultonEverything
            ? "pcall(function() if mvars then mvars.ply_allways_100percent_fulton=true end end)"
            : "pcall(function() if mvars then mvars.ply_allways_100percent_fulton=false end end)");
        pFulAll = c.fultonEverything;
    }

    // ══════════════════════════════════════════════════════════════════
    //  SECTION 2: REMOVED — these used fabricated field names.
    //  The toggles remain in the UI for future memory-patch support.
    //
    //  Noise scale:        vars.ply_noiseLevelRate (doesn't exist)
    //  Silent weapons:     vars.ply_isNoWeaponNoise (doesn't exist)
    //  Enemy sight/hear:   gvars.soldierSightDistRate (doesn't exist;
    //                      IH uses TppSoldier2.ReloadSoldier2ParameterTables
    //                      with modified Lua tables — too complex for us)
    //  Disable radio:      gvars.ene_disableRadioCall (doesn't exist)
    //  No alert prop:      gvars.ene_noAlertPropagation (doesn't exist)
    //  Disable game over:  mvars.mis_isDisableGameOver (doesn't exist;
    //                      IH hooks TppMission.ReserveGameOver)
    //  No marking:         gvars.mis_noMarking (doesn't exist)
    //  Subsistence:        mvars.mis_isSubsistence (doesn't exist;
    //                      subsistence is per-mission, not a runtime flag)
    //  Enemy phase:        TppMission.SetPhase (doesn't exist; phases are
    //                      per-CP via SendCommand, IH has its own system)
    //  Buddy equip IDs:    vars.quietWeaponId etc. (don't exist)
    //  Demo soldier:       vars.demoIsUseSoldier (doesn't exist; IH Ivar)
    //  MB characters:      gvars.mb_isEnableOcelot etc. (don't exist; IH)
    //  Vehicle god/ammo:   vars.veh_isInvincible etc. (don't exist)
    //  Skulls free roam:   gvars.skl_isEnableSkulls (doesn't exist; IH)
    //  Disable AI:         gvars.ene_isDisableAI (doesn't exist; memory
    //                      patch in world.cpp is the real implementation)
    //  Unlock weapons:     TppMotherBaseManagement.UnlockAllWeaponBlueprint
    //                      (doesn't exist in the game)
    //  Unlock missions:    TppMission.UnlockAllMission (doesn't exist)
    //  Unlock side ops:    TppQuest.UnlockAllQuest (doesn't exist)
    //  Force quest:        TppQuest.ForceStartQuest (doesn't exist)
    //  Play cassette:      TppUiCommand.PlayCassette (doesn't exist)
    //  Night vision:       GrTools.SetBrightness (doesn't exist)
    //  Avatar gender:      vars.avatarGender (not in deminified source)
    //  Body variation:     vars.playerBodyId (not in deminified source)
    // ══════════════════════════════════════════════════════════════════
}

// ── Field / function probe ──────────────────────────────────────────────────
// The entire probe runs as ONE Lua chunk on the GAME THREAD (via RunCode) to
// avoid the freeze caused by 40+ synchronous render-thread Lua calls. It stores
// its output in a Lua global (IH_PROBE_RESULT); the next Tick picks it up and
// prints it to the Lua Console.

// NOTE: builds IH_PROBE_RESULT INCREMENTALLY (string append on every line) so
// the instant the chunk runs at all, there is a non-empty result to harvest —
// even if a later line errors. No string.format (Fox Engine Lua may lack %s
// width), no table.concat in the critical path. If we still time out after
// this, the chunk literally never ran (compile/queue problem), not a Lua bug.
static const char* kProbeLua = R"LUA(
IH_PROBE_RESULT = '=== PROBE START ===\n'
local function add(s) IH_PROBE_RESULT = IH_PROBE_RESULT .. tostring(s) .. '\n' end
local function pad(s, n) s = tostring(s); while #s < n do s = s .. ' ' end; return s end

local ok, err = pcall(function()
  add('=== FIELD/FUNCTION PROBE  (FOUND = exists on this build) ===')

  local function test(label, ns_name, field, is_func)
    local ok2, res = pcall(function()
      local ns = _G[ns_name]
      if not ns then return 'absent (namespace nil)' end
      if is_func then return type(ns[field]) == 'function' and 'FOUND' or 'absent'
      else return ns[field] ~= nil and 'FOUND' or 'absent' end
    end)
    local status = ok2 and res or ('ERROR: ' .. tostring(res))
    add(pad(label, 18) .. ' ' .. ns_name .. '.' .. field .. ' : ' .. status)
  end

  test('Noise scale',       'vars',  'ply_noiseLevelRate', false)
  test('Noise scale',       'vars',  'playerNoiseRate',    false)
  test('Silent weapons',    'vars',  'ply_isNoWeaponNoise',false)
  test("Dont sub Hero",     'gvars', 'heroSubtractDisable',false)
  test("Dont add Demon",    'gvars', 'ogreAddDisable',     false)
  test('Enemy sight',       'gvars', 'soldierSightDistRate',false)
  test('Enemy sight fn',    'TppSoldier2','SetSightParam',  true)
  test('Enemy hearing',     'gvars', 'soldierHearingRate',  false)
  test('Enemy hearing fn',  'TppSoldier2','SetHearingParam', true)
  test('Enemy prep fn',     'TppRevenge','SetOspreyCombatGimmickCount', true)
  test('Enemy prep fn alt', 'TppRevenge','SetCombatGimmick', true)
  test('Disable radio',     'gvars', 'ene_disableRadioCall',false)
  test('No alert prop',     'gvars', 'ene_noAlertPropagation',false)
  test('Disable game over', 'mvars', 'mis_isDisableGameOver',false)
  test('No marking',        'gvars', 'mis_noMarking',       false)
  test('Subsistence',       'mvars', 'mis_isSubsistence',   false)
  test('Enemy phase fn',    'TppMission','SetPhase',         true)
  test('Discovery GO fn',   'TppMission','RegistDiscoveryGameOver', true)
  test('Buddy Quiet wpn',   'vars',  'quietWeaponId',       false)
  test('Buddy DD equip',    'vars',  'ddogEquipId',         false)
  test('Fulton count',      'vars',  'fultonCount',         false)
  test('Fulton everything', 'gvars', 'ful_isEnableFultonAll',false)
  test('Cutscene soldier',  'vars',  'demoIsUseSoldier',    false)
  test('MB Ocelot',         'gvars', 'mb_isEnableOcelot',   false)
  test('MB buddies',        'gvars', 'mb_isEnableBuddies',  false)
  test('Vehicle god',       'vars',  'veh_isInvincible',    false)
  test('Vehicle ammo',      'vars',  'veh_ammoCount',       false)
  test('Skulls free-roam',  'gvars', 'skl_isEnableSkulls',  false)
  test('No enemy AI',       'gvars', 'ene_isDisableAI',     false)
  test('Unlock weapons fn', 'TppMotherBaseManagement','UnlockAllWeaponBlueprint', true)
  test('Unlock items fn',   'TppMotherBaseManagement','UnlockAllItemBlueprint', true)
  test('Unlock missions fn','TppMission','UnlockAllMission', true)
  test('Unlock sideops fn', 'TppQuest','UnlockAllQuest',    true)
  test('Force quest fn',    'TppQuest','ForceStartQuest',   true)
  test('Cassette fn',       'TppUiCommand','PlayCassette',  true)
  test('Skip cutscene fn',  'TppDemo','Skip',               true)
  test('Night vision fn',   'GrTools','SetBrightness',      true)
  test('Clock scale fn',    'TppClock','SetTimeScale',      true)
  test('Marker getter',     'Tpp','GetMarkerPosition',      true)
  test('Marker getter alt', 'TppMarker2System','GetActiveMarkerPosition', true)
  test('Player warp fn',    'TppPlayer','Warp',             true)

  add('')
  add('=== NAMESPACE TYPES ===')
  local nsList = {'vars','gvars','mvars','TppSoldier2','TppRevenge','TppMission',
    'TppQuest','TppDemo','GrTools','TppClock','TppPlayer','TppUiCommand',
    'TppMotherBaseManagement','TppMarker2System','Tpp','Player','PlayerInfo',
    'GameObject','HighSpeedCamera','TppHelicopter','TppWeather',
    'TppReinforceBlock','TppCommandPost2','TppEnemyManager'}
  for i = 1, #nsList do
    local n = nsList[i]
    local ok3, tp = pcall(function() local v = _G[n] return v and type(v) or 'nil' end)
    add('  ' .. pad(n, 30) .. ' ' .. (ok3 and tp or ('ERROR: ' .. tostring(tp))))
  end

  add('')
  add('=== REAL KEYS (attempting pairs on each namespace) ===')
  local keywords = {'noise','hero','ogre','sight','hear','radio','alert','fulton',
    'mark','subsist','skull','disable','gameover','reflex','invinc','ammo','quiet',
    'ddog','dhorse','dwalker','ocelot','puppy','buddy','phase','reinforce','weapon',
    'suppress','stealth','speed','damage','recruit','staff','player'}
  local nsKeys = {'vars','gvars','mvars'}
  for i = 1, #nsKeys do
    local ns_name = nsKeys[i]
    local ok4, msg = pcall(function()
      local t = _G[ns_name]
      if t == nil then return '(nil)' end
      local tp = type(t)
      if tp ~= 'table' then return '(type=' .. tp .. ', pairs not supported)' end
      local found = {}
      for k,v in pairs(t) do
        if type(k) == 'string' then
          local lk = string.lower(k)
          for j = 1, #keywords do
            if string.find(lk, keywords[j], 1, true) then
              found[#found+1] = k .. '=' .. tostring(v)
              break
            end
          end
        end
      end
      if #found == 0 then return '(no matching string keys)' end
      table.sort(found)
      return '\n  ' .. table.concat(found, '\n  ')
    end)
    add(ns_name .. ': ' .. (ok4 and msg or ('ERROR: ' .. tostring(msg))))
  end
end)

if not ok then add('FATAL ERROR: ' .. tostring(err)) end
add('=== probe complete ===')
)LUA";

static ULONGLONG s_ProbeStartTime = 0;

void ProbeFields() {
    LuaConsole::Open();
    if (!IsReady()) {
        LuaConsole::PrintLine("[probe] Lua bridge not connected - load into a save first.", 2);
        return;
    }
    LuaConsole::PrintLine("[probe] Running on game thread... results appear shortly.", 1);
    // Pre-marker: if the big chunk below fails to COMPILE, this leaves a
    // detectable result so we know the queue/harvest pipeline itself works.
    RunCode("IH_PROBE_RESULT = '=== pipeline OK but main probe chunk failed to COMPILE ===\\n'");
    RunCode(kProbeLua);
    s_ProbePending = true;
    s_ProbeStartTime = GetTickCount64();
}

// Called from Tick() (render thread). Reads the C++ buffer that DrainQueue
// (game thread) filled — NO Lua calls from the render thread, zero race risk.
static void PollProbeResult() {
    if (!s_ProbePending) return;

    // Check timeout.
    ULONGLONG now = GetTickCount64();
    if (now - s_ProbeStartTime > 10000) {
        s_ProbePending = false;
        LuaConsole::PrintLine("[probe] Timed out - the Lua chunk may have errored.", 2);
        LuaConsole::PrintLine("[probe] Try the Lua Console manually: type(vars)", 2);
        return;
    }

    // Check if the game thread harvested a result.
    std::string r;
    EnterCriticalSection(&s_queueLock);
    if (s_ProbeResultReady) {
        r = std::move(s_ProbeResultBuf);
        s_ProbeResultReady = false;
    }
    LeaveCriticalSection(&s_queueLock);
    if (r.empty()) return;     // not ready yet

    s_ProbePending = false;

    // Split on newlines and print each line to the console.
    size_t pos = 0;
    while (pos < r.size()) {
        size_t nl = r.find('\n', pos);
        std::string line = (nl == std::string::npos) ? r.substr(pos) : r.substr(pos, nl - pos);
        int kind = 0;
        if (line.find("===") != std::string::npos)       kind = 1;
        else if (line.find("FOUND") != std::string::npos) kind = 3;
        else if (line.find("ERROR") != std::string::npos) kind = 2;
        LuaConsole::PrintLine(line.c_str(), kind);
        pos = (nl == std::string::npos) ? r.size() : nl + 1;
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
