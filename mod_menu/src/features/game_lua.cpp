#include "game_lua.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <cstdio>
#include <cstdlib>
#include <windows.h>

namespace Features::GameLua {

struct lua_State;
using lua_pcall_t       = int(__fastcall*)(lua_State* L, int nargs, int nresults, int errfunc);
using luaL_loadstring_t = int(__fastcall*)(lua_State* L, const char* s);
using lua_settop_t      = void(__fastcall*)(lua_State* L, int idx);
using lua_tolstring_t   = const char*(__fastcall*)(lua_State* L, int idx, size_t* len);

static lua_pcall_t       s_pcall      = nullptr;
static luaL_loadstring_t s_loadstring = nullptr;
static lua_settop_t      s_settop     = nullptr;
static lua_tolstring_t   s_tolstring  = nullptr;

static uintptr_t s_LuaStatePtr = 0;
static lua_State* s_L          = nullptr;

static int s_ScanFound = 0;
static int s_ScanTotal = 5;

static lua_State* GetL() {
    if (!s_L && s_LuaStatePtr)
        s_L = Memory::Read<lua_State*>(s_LuaStatePtr);
    return s_L;
}

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();
    s_ScanFound = 0;

    uintptr_t pcallScan = Pattern::Scan("56 48 83 EC 30 44 89 C6 4C 8B 49", base, size);
    if (pcallScan) { s_pcall = reinterpret_cast<lua_pcall_t>(pcallScan); ++s_ScanFound; }

    uintptr_t loadScan = Pattern::Scan("48 89 D6 48 89 CF E8 ?? ?? ?? ?? 48 89 F2 44 89 C1", base, size);
    if (loadScan) { s_loadstring = reinterpret_cast<luaL_loadstring_t>(loadScan); ++s_ScanFound; }

    uintptr_t settopScan = Pattern::Scan("85 D2 78 ?? 48 8B 41 ?? 48 8D 04 D0", base, size);
    if (settopScan) { s_settop = reinterpret_cast<lua_settop_t>(settopScan); ++s_ScanFound; }

    uintptr_t tolstrScan = Pattern::Scan("53 48 83 EC 20 89 D3 48 89 CF E8 ?? ?? ?? ?? 83 78 ?? 04", base, size);
    if (tolstrScan) { s_tolstring = reinterpret_cast<lua_tolstring_t>(tolstrScan); ++s_ScanFound; }

    uintptr_t stateScan = Pattern::Scan(
        "48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? BA 01",
        base, size);
    if (stateScan) {
        int32_t off = Memory::Read<int32_t>(stateScan + 3);
        s_LuaStatePtr = stateScan + 7 + off;
        ++s_ScanFound;
    }
}

bool IsReady() {
    return GetL() && s_pcall && s_loadstring;
}

void RunCode(const char* luaCode) {
    lua_State* L = GetL();
    if (!L || !s_pcall || !s_loadstring) return;

    if (s_loadstring(L, luaCode) != 0) {
        if (s_settop) s_settop(L, 0);
        return;
    }
    if (s_pcall(L, 0, 0, 0) != 0) {
        if (s_settop) s_settop(L, 0);
        return;
    }
}

int RunCodeInt(const char* luaCode, int fallback) {
    lua_State* L = GetL();
    if (!L || !s_pcall || !s_loadstring) return fallback;

    if (s_loadstring(L, luaCode) != 0) {
        if (s_settop) s_settop(L, 0);
        return fallback;
    }
    if (s_pcall(L, 0, 1, 0) != 0) {
        if (s_settop) s_settop(L, 0);
        return fallback;
    }

    int result = fallback;
    if (s_tolstring) {
        const char* s = s_tolstring(L, -1, nullptr);
        if (s) result = atoi(s);
    }
    if (s_settop) s_settop(L, 0);
    return result;
}

int GetScanFound() { return s_ScanFound; }
int GetScanTotal() { return s_ScanTotal; }

static void RunIfChanged(int& prev, int current, const char* fmt) {
    if (current == prev) return;
    prev = current;
    char buf[256];
    snprintf(buf, sizeof(buf), fmt, current);
    RunCode(buf);
}

static void RunIfBoolChanged(bool& prev, bool current, const char* onCode, const char* offCode) {
    if (current == prev) return;
    prev = current;
    RunCode(current ? onCode : offCode);
}

void Tick() {
    if (!IsReady()) return;
    auto& c = Config::Get();

    // ── God mode via base-game vars ────────────────────────────────────
    // vars.noDamageMode is a real game variable checked during damage calc.
    // Reassert periodically because the game resets it on mission transitions.
    static bool prevGod = false;
    static ULONGLONG lastGodAssert = 0;
    ULONGLONG now = GetTickCount64();
    if (c.godMode != prevGod || (c.godMode && now - lastGodAssert > 2000)) {
        RunCode(c.godMode ? "vars.noDamageMode=1" : "vars.noDamageMode=0");
        prevGod = c.godMode;
        lastGodAssert = now;
    }

    // ── Infinite ammo — try known Lua paths ────────────────────────────
    static bool prevInfAmmo = false;
    if (c.infiniteAmmo != prevInfAmmo) {
        RunCode(c.infiniteAmmo
            ? "pcall(function() Player.SetInfiniteAmmo(true) end) "
              "pcall(function() GkEquip.SetInfiniteAmmo(true) end)"
            : "pcall(function() Player.SetInfiniteAmmo(false) end) "
              "pcall(function() GkEquip.SetInfiniteAmmo(false) end)");
        prevInfAmmo = c.infiniteAmmo;
    }

    // ── Stealth mode ───────────────────────────────────────────────────
    static bool prevStealth = false;
    if (c.stealthMode != prevStealth) {
        RunCode(c.stealthMode
            ? "pcall(function() vars.stealthCamo=1 end) "
              "pcall(function() TppPlayer.SetDiscoveryRate(0) end)"
            : "pcall(function() vars.stealthCamo=0 end) "
              "pcall(function() TppPlayer.SetDiscoveryRate(1.0) end)");
        prevStealth = c.stealthMode;
    }

    // ── Super speed ────────────────────────────────────────────────────
    static bool prevSpeed = false;
    static float prevSpeedVal = 1.0f;
    if (c.superSpeed != prevSpeed || (c.superSpeed && c.speedMultiplier != prevSpeedVal)) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "pcall(function() Player.SetMoveSpeedRate(%.2f) end)",
            c.superSpeed ? c.speedMultiplier : 1.0f);
        RunCode(buf);
        prevSpeed = c.superSpeed;
        prevSpeedVal = c.speedMultiplier;
    }

    // ── Time scale (slow motion) ───────────────────────────────────────
    static bool prevSlow = false;
    static float prevTimeScale = 1.0f;
    if (c.slowMotion != prevSlow || (c.slowMotion && c.timeScale != prevTimeScale)) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "pcall(function() TppClock.SetNowTimeScale(%.4f) end)",
            c.slowMotion ? c.timeScale : 1.0f);
        RunCode(buf);
        prevSlow = c.slowMotion;
        prevTimeScale = c.timeScale;
    }

    // ── Time of day ────────────────────────────────────────────────────
    static bool prevOverride = false;
    static float prevTOD = 12.0f;
    if (c.overrideTime != prevOverride || (c.overrideTime && c.timeOfDay != prevTOD)) {
        if (c.overrideTime) {
            char buf[128];
            snprintf(buf, sizeof(buf),
                "pcall(function() TppClock.SetRealTime(%.2f) end)", c.timeOfDay);
            RunCode(buf);
        }
        prevOverride = c.overrideTime;
        prevTOD = c.timeOfDay;
    }

    // ── Weather ────────────────────────────────────────────────────────
    static int prevWeather = -1;
    if (c.weatherOverride) {
        static const char* weatherLua[] = {
            "",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.SUNNY)",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.CLOUDY)",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.RAINY)",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.SANDSTORM)",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.FOGGY)",
            "TppWeather.SetCurrentWeather(TppDefine.WEATHER.POURING)",
        };
        if (c.weather != prevWeather && c.weather > 0 && c.weather < 7) {
            RunCode(weatherLua[c.weather]);
            prevWeather = c.weather;
        }
    }

    // ── Helicopter ─────────────────────────────────────────────────────
    static bool prevInvHeli = false;
    RunIfBoolChanged(prevInvHeli, c.invincibleHeli,
        "TppHelicopter.SetInvincible(true)", "TppHelicopter.SetInvincible(false)");

    static int prevLight = -1;
    if (c.searchLight != prevLight) {
        if (c.searchLight == 1) RunCode("TppHelicopter.SetSearchLight(false)");
        else if (c.searchLight == 2) RunCode("TppHelicopter.SetSearchLight(true)");
        prevLight = c.searchLight;
    }

    // ── Player health scale ────────────────────────────────────────────
    static int prevHpScale = 100;
    if (c.playerHealthScale != prevHpScale) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "pcall(function() Player.SetMaxLifePoint(%d) end)", c.playerHealthScale * 100);
        RunCode(buf);
        prevHpScale = c.playerHealthScale;
    }

    // ── Mission ────────────────────────────────────────────────────────
    static bool prevGOD = false;
    RunIfBoolChanged(prevGOD, c.gameOverOnDiscovery,
        "TppMission.SetGameOverOnDiscovery(true)",
        "TppMission.SetGameOverOnDiscovery(false)");

    // ── Resources via Lua (locks) ──────────────────────────────────────
    // Reassert every 2s when locked rather than every frame.
    static ULONGLONG lastResAssert = 0;
    static int prevGmpLua = -1, prevHeroLua = -1, prevDemonLua = -1;
    bool resTimer = (now - lastResAssert > 2000);

    if (c.lockGMP && (c.gmp != prevGmpLua || resTimer)) {
        char buf[160];
        snprintf(buf, sizeof(buf),
            "pcall(function() TppTerminal.AcquireGmp(%d - (TppTerminal.GetGmp() or 0)) end)",
            c.gmp);
        RunCode(buf);
        prevGmpLua = c.gmp;
    }
    if (c.lockHeroism && (c.heroism != prevHeroLua || resTimer)) {
        char buf[160];
        snprintf(buf, sizeof(buf),
            "pcall(function() TppTerminal.AcquireHeroicPoint(%d - (TppTerminal.GetHeroicPoint() or 0)) end)",
            c.heroism);
        RunCode(buf);
        prevHeroLua = c.heroism;
    }
    if (c.lockDemonPoints && (c.demonPoints != prevDemonLua || resTimer)) {
        char buf[128];
        snprintf(buf, sizeof(buf),
            "pcall(function() vars.demonPoint=%d end)", c.demonPoints);
        RunCode(buf);
        prevDemonLua = c.demonPoints;
    }
    if (resTimer) lastResAssert = now;

    // ── Appearance — player type ───────────────────────────────────────
    static int prevType = -1;
    if (c.playerType != prevType) {
        char buf[256];
        snprintf(buf, sizeof(buf),
            "pcall(function() "
            "vars.playerType=%d "
            "if TppPlayer and TppPlayer.ResetPlayerType then TppPlayer.ResetPlayerType() end "
            "end)", c.playerType);
        RunCode(buf);
        prevType = c.playerType;
    }

    // ── No fall damage ─────────────────────────────────────────────────
    static bool prevNoFall = false;
    static ULONGLONG lastNoFall = 0;
    if (c.noFallDamage != prevNoFall || (c.noFallDamage && now - lastNoFall > 2000)) {
        RunCode(c.noFallDamage
            ? "pcall(function() vars.noFallDamage=1 end)"
            : "pcall(function() vars.noFallDamage=0 end)");
        prevNoFall = c.noFallDamage;
        lastNoFall = now;
    }

    // ── Infinite reflex ────────────────────────────────────────────────
    static bool prevInfReflex = false;
    if (c.infiniteReflex != prevInfReflex) {
        RunCode(c.infiniteReflex
            ? "pcall(function() vars.reflexModeTime=999999 end)"
            : "pcall(function() vars.reflexModeTime=nil end)");
        prevInfReflex = c.infiniteReflex;
    }
}

void RunDiagnostics() {
    if (!IsReady()) {
        RunCode("pcall(function() TppUiCommand.AnnounceLogView('IH: Lua bridge NOT connected') end)");
        return;
    }
    RunCode(
        "pcall(function() TppUiCommand.AnnounceLogView('IH: Lua bridge OK') end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: vars table ' .. (vars and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppWeather ' .. (TppWeather and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppTerminal ' .. (TppTerminal and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppClock ' .. (TppClock and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: Player ' .. (Player and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppPlayer ' .. (TppPlayer and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: GkEquip ' .. (GkEquip and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppHelicopter ' .. (TppHelicopter and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppMission ' .. (TppMission and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: TppDefine ' .. (TppDefine and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: GkBuddy ' .. (GkBuddy and 'OK' or 'MISSING')) end) "
        "pcall(function() TppUiCommand.AnnounceLogView('IH: GkEnemy ' .. (GkEnemy and 'OK' or 'MISSING')) end)"
    );
}

} // namespace Features::GameLua
