#include "lua_console.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <cstring>

namespace Features::LuaConsole {

struct lua_State;
using lua_pcall_t       = int(__fastcall*)(lua_State* L, int nargs, int nresults, int errfunc);
using luaL_loadstring_t = int(__fastcall*)(lua_State* L, const char* s);
using lua_tolstring_t   = const char*(__fastcall*)(lua_State* L, int idx, size_t* len);
using lua_settop_t      = void(__fastcall*)(lua_State* L, int idx);

static lua_pcall_t       s_pcall      = nullptr;
static luaL_loadstring_t s_loadstring = nullptr;
static lua_tolstring_t   s_tolstring  = nullptr;
static lua_settop_t      s_settop     = nullptr;
static lua_State*        s_L          = nullptr;

struct LogEntry { std::string text; ImVec4 color; };
static std::vector<LogEntry> s_Log;
static char s_Buf[512] = {};
static std::vector<std::string> s_History;
static int s_HistIdx = -1;
static bool s_Scroll = false;

static ImVec4 COL_IN  = ImVec4(0.70f, 0.85f, 0.65f, 1.0f);
static ImVec4 COL_OUT = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
static ImVec4 COL_ERR = ImVec4(1.00f, 0.40f, 0.40f, 1.0f);
static ImVec4 COL_SYS = ImVec4(0.50f, 0.70f, 0.50f, 1.0f);

static void Log(const std::string& t, ImVec4 c) { s_Log.push_back({t,c}); s_Scroll = true; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t pc = Pattern::Scan("56 48 83 EC 30 44 89 C6 4C 8B 49", base, size);
    if (pc) s_pcall = reinterpret_cast<lua_pcall_t>(pc);

    uintptr_t ls = Pattern::Scan("48 89 D6 48 89 CF E8 ?? ?? ?? ?? 48 89 F2 44 89 C1", base, size);
    if (ls) s_loadstring = reinterpret_cast<luaL_loadstring_t>(ls);

    uintptr_t tl = Pattern::Scan("53 48 83 EC 20 89 D3 48 89 CF E8 ?? ?? ?? ?? 83 78 ?? 04", base, size);
    if (tl) s_tolstring = reinterpret_cast<lua_tolstring_t>(tl);

    uintptr_t st = Pattern::Scan("85 D2 78 ?? 48 8B 41 ?? 48 8D 04 D0", base, size);
    if (st) s_settop = reinterpret_cast<lua_settop_t>(st);

    uintptr_t ss = Pattern::Scan("48 8B 0D ?? ?? ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? BA 01", base, size);
    if (ss) {
        int32_t off = Memory::Read<int32_t>(ss + 3);
        s_L = Memory::Read<lua_State*>(ss + 7 + off);
    }

    if (s_L && s_pcall && s_loadstring)
        Log("Lua console ready - Fox Engine Lua 5.1", COL_SYS);
    else
        Log("Lua: some functions not resolved", COL_SYS);

    Log("Examples: Player.SetMaxHp(100000)  TppWeather.SetCurrentWeather(TppDefine.WEATHER.SUNNY)", COL_SYS);
}

static void Execute(const char* code) {
    Log("> " + std::string(code), COL_IN);
    if (!s_L || !s_pcall || !s_loadstring) { Log("[error] Lua not connected", COL_ERR); return; }

    if (s_loadstring(s_L, code) != 0) {
        if (s_tolstring) { const char* e = s_tolstring(s_L, -1, nullptr); Log(e ? e : "Compile error", COL_ERR); }
        if (s_settop) s_settop(s_L, 0);
        return;
    }
    if (s_pcall(s_L, 0, 1, 0) != 0) {
        if (s_tolstring) { const char* e = s_tolstring(s_L, -1, nullptr); Log(e ? e : "Runtime error", COL_ERR); }
        if (s_settop) s_settop(s_L, 0);
        return;
    }
    if (s_tolstring) {
        const char* r = s_tolstring(s_L, -1, nullptr);
        if (r && strlen(r) > 0) Log(r, COL_OUT);
    }
    if (s_settop) s_settop(s_L, 0);
}

static int InputCB(ImGuiInputTextCallbackData* d) {
    if (d->EventFlag == ImGuiInputTextFlags_CallbackHistory && !s_History.empty()) {
        if (d->EventKey == ImGuiKey_UpArrow) {
            s_HistIdx = (s_HistIdx < 0) ? (int)s_History.size()-1 : (s_HistIdx > 0 ? s_HistIdx-1 : s_HistIdx);
        } else if (d->EventKey == ImGuiKey_DownArrow) {
            if (s_HistIdx >= 0) s_HistIdx++;
            if (s_HistIdx >= (int)s_History.size()) s_HistIdx = -1;
        }
        const char* h = s_HistIdx >= 0 ? s_History[s_HistIdx].c_str() : "";
        d->DeleteChars(0, d->BufTextLen);
        d->InsertChars(0, h);
    }
    return 0;
}

void RenderWindow() {
    auto& c = Config::Get();
    ImGui::SetNextWindowPos(ImVec2(400, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Lua Console##luawin", &c.luaConsoleOpen, ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        return;
    }

    // Quick buttons
    if (ImGui::Button("Max HP")) Execute("Player.SetMaxHp(100000)");
    ImGui::SameLine();
    if (ImGui::Button("Supply")) Execute("TppMission.ReserveSupplyDrop()");
    ImGui::SameLine();
    if (ImGui::Button("Clear Weather")) Execute("TppWeather.SetCurrentWeather(TppDefine.WEATHER.SUNNY)");
    ImGui::SameLine();
    if (ImGui::Button("Clear")) s_Log.clear();

    ImGui::Separator();

    float foot = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("##scroll", ImVec2(0, -foot), true);
    for (auto& e : s_Log) {
        ImGui::PushStyleColor(ImGuiCol_Text, e.color);
        ImGui::TextWrapped("%s", e.text.c_str());
        ImGui::PopStyleColor();
    }
    if (s_Scroll) { ImGui::SetScrollHereY(1.0f); s_Scroll = false; }
    ImGui::EndChild();

    ImGuiInputTextFlags fl = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
    ImGui::SetNextItemWidth(-50);
    bool go = ImGui::InputText("##in", s_Buf, sizeof(s_Buf), fl, InputCB);
    ImGui::SameLine();
    go |= ImGui::Button("Run");

    if (go && s_Buf[0]) {
        s_History.push_back(s_Buf);
        s_HistIdx = -1;
        Execute(s_Buf);
        s_Buf[0] = 0;
    }

    ImGui::End();
}

} // namespace Features::LuaConsole
