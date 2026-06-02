#include "lua_console.h"
#include "../config.h"
#include "game_lua.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <cstring>

namespace Features::LuaConsole {

// NOTE: this console no longer owns a Lua bridge of its own. It delegates to
// Features::GameLua, which is the single bridge in the mod. That bridge runs
// queued code on the GAME thread (see game_lua.cpp); executing Lua from the
// render thread, as this file used to, races the VM and silently fails.

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

void PrintLine(const char* text, int kind) {
    ImVec4 c = (kind == 1) ? COL_SYS : (kind == 2) ? COL_ERR
             : (kind == 3) ? ImVec4(0.55f, 0.90f, 0.55f, 1.0f) : COL_OUT;
    Log(text ? text : "", c);
}

void Open() { Config::Get().luaConsoleOpen = true; }

void Init() {
    Log("Code runs on the game thread. To see output in-game, call "
        "TppUiCommand.AnnounceLogView(tostring(x)).", COL_SYS);
    Log("Examples: Player.ChangeLifeMaxValue(50000)  TppWeather.ForceRequestWeather(1,2.0)", COL_SYS);
}

static void Execute(const char* code) {
    Log("> " + std::string(code), COL_IN);
    if (!Features::GameLua::IsReady()) {
        Log("[error] Lua bridge not connected (see Pattern Scan Report)", COL_ERR);
        return;
    }
    // Delegate to the single game-thread bridge. Wrap as a statement (so
    // assignments like `vars.x=5` work) and surface any runtime error via the
    // in-game log. To print an expression's value, type it as
    // `TppUiCommand.AnnounceLogView(tostring(<expr>))`.
    std::string wrapped =
        "do local ok,err = pcall(function() " + std::string(code) + " end) "
        "if not ok and TppUiCommand and TppUiCommand.AnnounceLogView then "
        "TppUiCommand.AnnounceLogView('IH ERR: '..tostring(err)) end end";
    Features::GameLua::RunCode(wrapped.c_str());
    Log("[queued -> game thread; errors show in the in-game log]", COL_SYS);
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

    // Live bridge status (updates every frame — no stale Init() message)
    bool ready = Features::GameLua::IsReady();
    if (ready) {
        ImGui::TextColored(ImVec4(0.4f,0.9f,0.4f,1.0f), "LUA BRIDGE: CONNECTED");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.0f),
            "(%d/%d scans)", Features::GameLua::GetScanFound(), Features::GameLua::GetScanTotal());
    } else {
        ImGui::TextColored(ImVec4(1.0f,0.4f,0.4f,1.0f), "LUA BRIDGE: NOT CONNECTED");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.0f), "(load into a mission/FOB)");
    }
    ImGui::Separator();

    // Quick buttons
    if (ImGui::Button("Max HP")) Execute("pcall(function() Player.ResetLifeMaxValue() Player.ChangeLifeMaxValue(50000) end)");
    ImGui::SameLine();
    if (ImGui::Button("5M GMP")) Execute("pcall(function() TppMotherBaseManagement.SetGmp{gmp=5000000} end)");
    ImGui::SameLine();
    if (ImGui::Button("Sunny")) Execute("pcall(function() TppWeather.ForceRequestWeather(1,2.0) end)");
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
