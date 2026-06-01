#include "game_lua.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <cstdio>
#include <cstdlib>

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
static lua_State*        s_L          = nullptr;

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t pcallScan = Pattern::Scan("56 48 83 EC 30 44 89 C6 4C 8B 49", base, size);
    if (pcallScan) s_pcall = reinterpret_cast<lua_pcall_t>(pcallScan);

    uintptr_t loadScan = Pattern::Scan("48 89 D6 48 89 CF E8 ?? ?? ?? ?? 48 89 F2 44 89 C1", base, size);
    if (loadScan) s_loadstring = reinterpret_cast<luaL_loadstring_t>(loadScan);

    uintptr_t settopScan = Pattern::Scan("85 D2 78 ?? 48 8B 41 ?? 48 8D 04 D0", base, size);
    if (settopScan) s_settop = reinterpret_cast<lua_settop_t>(settopScan);

    uintptr_t tolstrScan = Pattern::Scan("53 48 83 EC 20 89 D3 48 89 CF E8 ?? ?? ?? ?? 83 78 ?? 04", base, size);
    if (tolstrScan) s_tolstring = reinterpret_cast<lua_tolstring_t>(tolstrScan);

    uintptr_t stateScan = Pattern::Scan(
        "48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? BA 01",
        base, size);
    if (stateScan) {
        int32_t off = Memory::Read<int32_t>(stateScan + 3);
        uintptr_t ptr = stateScan + 7 + off;
        s_L = Memory::Read<lua_State*>(ptr);
    }
}

bool IsReady() {
    return s_L && s_pcall && s_loadstring;
}

void RunCode(const char* luaCode) {
    if (!s_L || !s_pcall || !s_loadstring) return;

    if (s_loadstring(s_L, luaCode) != 0) {
        if (s_settop) s_settop(s_L, 0);
        return;
    }
    if (s_pcall(s_L, 0, 0, 0) != 0) {
        if (s_settop) s_settop(s_L, 0);
        return;
    }
}

int RunCodeInt(const char* luaCode, int fallback) {
    if (!s_L || !s_pcall || !s_loadstring) return fallback;

    if (s_loadstring(s_L, luaCode) != 0) {
        if (s_settop) s_settop(s_L, 0);
        return fallback;
    }
    if (s_pcall(s_L, 0, 1, 0) != 0) {
        if (s_settop) s_settop(s_L, 0);
        return fallback;
    }

    int result = fallback;
    if (s_tolstring) {
        const char* s = s_tolstring(s_L, -1, nullptr);
        if (s) result = atoi(s);
    }
    if (s_settop) s_settop(s_L, 0);
    return result;
}

static void RunIfChanged(int& prev, int current, const char* fmt) {
    if (current == prev) return;
    prev = current;
    char buf[256];
    snprintf(buf, sizeof(buf), fmt, current);
    RunCode(buf);
}

void Tick() {
    if (!s_L) return;
    auto& c = Config::Get();

    // Appearance — player type
    static int prevType = -1;
    RunIfChanged(prevType, c.playerType, "vars.playerType=%d");

    // Weather override
    static int prevWeather = -1;
    if (c.weatherOverride) {
        static const char* weatherLua[] = {
            "", // NONE
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

    // Heli invincibility
    static bool prevInvHeli = false;
    if (c.invincibleHeli != prevInvHeli) {
        RunCode(c.invincibleHeli ? "TppHelicopter.SetInvincible(true)" : "TppHelicopter.SetInvincible(false)");
        prevInvHeli = c.invincibleHeli;
    }

    // Heli search light
    static int prevLight = -1;
    if (c.searchLight != prevLight) {
        if (c.searchLight == 1) RunCode("TppHelicopter.SetSearchLight(false)");
        else if (c.searchLight == 2) RunCode("TppHelicopter.SetSearchLight(true)");
        prevLight = c.searchLight;
    }

    // Player health scale via Lua
    static int prevHpScale = 100;
    if (c.playerHealthScale != prevHpScale) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Player.SetMaxHp(%d)", c.playerHealthScale * 100);
        RunCode(buf);
        prevHpScale = c.playerHealthScale;
    }

    // Game over on discovery
    static bool prevGOD = false;
    if (c.gameOverOnDiscovery != prevGOD) {
        RunCode(c.gameOverOnDiscovery ? "TppMission.SetGameOverOnDiscovery(true)" : "TppMission.SetGameOverOnDiscovery(false)");
        prevGOD = c.gameOverOnDiscovery;
    }
}

} // namespace Features::GameLua
