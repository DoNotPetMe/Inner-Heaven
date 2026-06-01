#include "lua_console.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <cstring>

// MGSV:TPP embeds Lua 5.1. We hook lua_newstate to capture the game's
// Lua state pointer, then call lua_pcall / luaL_loadstring to execute
// arbitrary Lua code. This lets users run any Fox Engine Lua command
// from inside the mod menu — a unique feature not found in standalone
// CE tables or basic trainers.

namespace Features::LuaConsole {

// Lua 5.1 C API typedefs (matching the embedded Lua in Fox Engine)
struct lua_State;
using lua_CFunction = int(*)(lua_State*);

using lua_newstate_t   = lua_State*(__fastcall*)(void* allocFunc, void* ud);
using lua_pcall_t      = int(__fastcall*)(lua_State* L, int nargs, int nresults, int errfunc);
using luaL_loadstring_t = int(__fastcall*)(lua_State* L, const char* s);
using lua_tolstring_t  = const char*(__fastcall*)(lua_State* L, int idx, size_t* len);
using lua_settop_t     = void(__fastcall*)(lua_State* L, int idx);
using lua_type_t       = int(__fastcall*)(lua_State* L, int idx);
using lua_toboolean_t  = int(__fastcall*)(lua_State* L, int idx);
using lua_tonumber_t   = double(__fastcall*)(lua_State* L, int idx);

static lua_pcall_t       s_lua_pcall      = nullptr;
static luaL_loadstring_t s_luaL_loadstring = nullptr;
static lua_tolstring_t   s_lua_tolstring   = nullptr;
static lua_settop_t      s_lua_settop      = nullptr;
static lua_type_t        s_lua_type        = nullptr;
static lua_toboolean_t   s_lua_toboolean   = nullptr;
static lua_tonumber_t    s_lua_tonumber    = nullptr;

static lua_State* s_LuaState = nullptr;

// Console log
struct LogEntry {
    std::string text;
    ImVec4 color;
};

static std::vector<LogEntry> s_Log;
static char s_InputBuf[512] = {};
static std::vector<std::string> s_History;
static int s_HistoryIdx = -1;
static bool s_ScrollToBottom = false;

static ImVec4 COL_INPUT  = ImVec4(0.70f, 0.85f, 0.65f, 1.0f);
static ImVec4 COL_OUTPUT = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
static ImVec4 COL_ERROR  = ImVec4(1.00f, 0.40f, 0.40f, 1.0f);
static ImVec4 COL_SYSTEM = ImVec4(0.50f, 0.70f, 0.50f, 1.0f);

static void LogSystem(const std::string& msg) {
    s_Log.push_back({ msg, COL_SYSTEM });
    s_ScrollToBottom = true;
}

static void LogInput(const std::string& msg) {
    s_Log.push_back({ "> " + msg, COL_INPUT });
    s_ScrollToBottom = true;
}

static void LogOutput(const std::string& msg) {
    s_Log.push_back({ msg, COL_OUTPUT });
    s_ScrollToBottom = true;
}

static void LogError(const std::string& msg) {
    s_Log.push_back({ "[error] " + msg, COL_ERROR });
    s_ScrollToBottom = true;
}

// Verified IHHook AOB patterns for Lua 5.1 API functions
// Source: mgsvtpp_patterns.h from TinManTex/IHHook
void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // lua_newstate: used to capture the game's Lua state
    // Pattern: "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 89 D7 4C 89 C5"
    uintptr_t newstateScan = Pattern::Scan(
        "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 48 89 D7 4C 89 C5",
        base, size
    );

    // lua_pcall
    uintptr_t pcallScan = Pattern::Scan(
        "56 48 83 EC 30 44 89 C6 4C 8B 49", base, size
    );
    if (pcallScan)
        s_lua_pcall = reinterpret_cast<lua_pcall_t>(pcallScan);

    // luaL_loadstring — calls lua_load internally
    uintptr_t loadstrScan = Pattern::Scan(
        "48 89 D6 48 89 CF E8 ?? ?? ?? ?? 48 89 F2 44 89 C1", base, size
    );
    if (loadstrScan)
        s_luaL_loadstring = reinterpret_cast<luaL_loadstring_t>(loadstrScan);

    // lua_tolstring
    uintptr_t tolstringScan = Pattern::Scan(
        "53 48 83 EC 20 89 D3 48 89 CF E8 ?? ?? ?? ?? 83 78 ?? 04", base, size
    );
    if (tolstringScan)
        s_lua_tolstring = reinterpret_cast<lua_tolstring_t>(tolstringScan);

    // lua_settop
    uintptr_t settopScan = Pattern::Scan(
        "85 D2 78 ?? 48 8B 41 ?? 48 8D 04 D0", base, size
    );
    if (settopScan)
        s_lua_settop = reinterpret_cast<lua_settop_t>(settopScan);

    // For now, the Lua state must be located dynamically.
    // IHHook hooks lua_newstate to intercept it. We scan for the state
    // pointer in the game's globals instead.
    // The Lua state is stored after the engine finishes initialization;
    // we'll try to resolve it from known data structures.

    // Scan for the global that holds the lua_State*
    uintptr_t luaStateScan = Pattern::Scan(
        "48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? BA 01",
        base, size
    );
    if (luaStateScan) {
        // RIP-relative address: next instruction + offset at scan+3
        int32_t offset = Memory::Read<int32_t>(luaStateScan + 3);
        uintptr_t statePtr = luaStateScan + 7 + offset;
        s_LuaState = Memory::Read<lua_State*>(statePtr);
    }

    if (s_LuaState && s_lua_pcall && s_luaL_loadstring)
        LogSystem("Lua console ready — Fox Engine Lua 5.1 state captured");
    else
        LogSystem("Lua console: some functions not found (patterns may need updating)");

    LogSystem("Type Lua code and press Enter. Examples:");
    LogSystem("  TppMission.SetMissionClearCount(\"10020\", 1)");
    LogSystem("  Player.SetMaxHp(100000)");
    LogSystem("  TppWeather.SetFogLevel(0)");
    LogSystem("  GkEventTimerManager.Start(\"Timer_Heli\", 0)");
}

static void Execute(const char* code) {
    LogInput(code);

    if (!s_LuaState) {
        LogError("Lua state not captured — game may need to finish loading");
        return;
    }
    if (!s_lua_pcall || !s_luaL_loadstring) {
        LogError("Lua API functions not resolved");
        return;
    }

    int loadResult = s_luaL_loadstring(s_LuaState, code);
    if (loadResult != 0) {
        if (s_lua_tolstring) {
            const char* err = s_lua_tolstring(s_LuaState, -1, nullptr);
            LogError(err ? err : "Compile error (unknown)");
            if (s_lua_settop) s_lua_settop(s_LuaState, -2);
        } else {
            LogError("Compile error");
        }
        return;
    }

    int callResult = s_lua_pcall(s_LuaState, 0, 1, 0);
    if (callResult != 0) {
        if (s_lua_tolstring) {
            const char* err = s_lua_tolstring(s_LuaState, -1, nullptr);
            LogError(err ? err : "Runtime error (unknown)");
            if (s_lua_settop) s_lua_settop(s_LuaState, -2);
        } else {
            LogError("Runtime error");
        }
        return;
    }

    // Try to print the return value
    if (s_lua_tolstring) {
        const char* result = s_lua_tolstring(s_LuaState, -1, nullptr);
        if (result && strlen(result) > 0)
            LogOutput(result);
    }

    if (s_lua_settop)
        s_lua_settop(s_LuaState, 0);
}

static int InputCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
        if (s_History.empty()) return 0;

        if (data->EventKey == ImGuiKey_UpArrow) {
            if (s_HistoryIdx < 0)
                s_HistoryIdx = static_cast<int>(s_History.size()) - 1;
            else if (s_HistoryIdx > 0)
                s_HistoryIdx--;
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (s_HistoryIdx >= 0)
                s_HistoryIdx++;
            if (s_HistoryIdx >= static_cast<int>(s_History.size()))
                s_HistoryIdx = -1;
        }

        const char* hist = (s_HistoryIdx >= 0) ? s_History[s_HistoryIdx].c_str() : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, hist);
    }
    return 0;
}

void RenderTab() {
    if (!ImGui::BeginTabItem("Lua Console"))
        return;

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Fox Engine Lua 5.1 Console");
    ImGui::SameLine(380);
    if (s_LuaState)
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.3f, 1.0f), "[Connected]");
    else
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), "[Disconnected]");

    ImGui::Separator();

    // Quick-execute buttons
    if (ImGui::Button("Max HP"))
        Execute("Player.SetMaxHp(100000)");
    ImGui::SameLine();
    if (ImGui::Button("Reflex ON"))
        Execute("Player.SetReflex(true)");
    ImGui::SameLine();
    if (ImGui::Button("Supply Drop"))
        Execute("TppMission.ReserveSupplyDrop()");
    ImGui::SameLine();
    if (ImGui::Button("Clear Weather"))
        Execute("TppWeather.SetCurrentWeather(TppDefine.WEATHER.SUNNY)");

    ImGui::Separator();

    // Log window
    float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("##luascroll", ImVec2(0, -footerHeight), true);

    for (auto& entry : s_Log) {
        ImGui::PushStyleColor(ImGuiCol_Text, entry.color);
        ImGui::TextWrapped("%s", entry.text.c_str());
        ImGui::PopStyleColor();
    }

    if (s_ScrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        s_ScrollToBottom = false;
    }

    ImGui::EndChild();

    // Input line
    ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue |
                                     ImGuiInputTextFlags_CallbackHistory;

    ImGui::SetNextItemWidth(-60);
    bool submitted = ImGui::InputText("##luainput", s_InputBuf, sizeof(s_InputBuf), inputFlags, InputCallback);
    ImGui::SameLine();
    submitted |= ImGui::Button("Run");

    if (submitted && s_InputBuf[0] != '\0') {
        std::string cmd(s_InputBuf);
        s_History.push_back(cmd);
        s_HistoryIdx = -1;
        Execute(cmd.c_str());
        s_InputBuf[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        s_Log.clear();

    ImGui::EndTabItem();
}

} // namespace Features::LuaConsole
