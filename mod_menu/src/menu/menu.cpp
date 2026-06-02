#include "menu.h"
#include "menu_system.h"
#include "menu_defs.h"
#include "../features/lua_console.h"
#include "../features/player.h"
#include "../features/resources.h"
#include "../features/world.h"
#include "../features/visuals.h"
#include "../features/game_lua.h"
#include "../config.h"
#include <imgui.h>

namespace Menu {

void Init() {
    MenuSystem::Get().SetRoot(BuildMenuTree());
}

static void RenderScanReport() {
    ImGui::SetNextWindowPos(ImVec2(420, 40), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(440, 520), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Pattern Scan Report##scanrep", &Config::Get().showScanReport,
                      ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        return;
    }

    const ImVec4 ok  = ImVec4(0.40f, 0.85f, 0.45f, 1.0f);
    const ImVec4 bad = ImVec4(0.95f, 0.40f, 0.40f, 1.0f);

    // ── Lua bridge (gates ALL Lua-based features) ──────────────────────
    bool luaReady = Features::GameLua::IsReady();
    ImGui::TextDisabled("LUA BRIDGE");
    ImGui::SameLine();
    ImGui::TextColored(luaReady ? ok : bad, luaReady ? "CONNECTED" : "NOT CONNECTED");
    ImGui::TextWrapped("Lua bridge drives ~60 features (god mode, resources, weather, "
                       "appearance, enemy, mission, buddy, etc). If this is red, none of "
                       "those work \xE2\x80\x94 the function signatures don't match your game "
                       "build. See STATUS.md; for working singleplayer cheats use Infinite "
                       "Heaven (IHHook), which keeps these addresses up to date.");
    ImGui::Text("  Lua scans: %d / %d",
                Features::GameLua::GetScanFound(), Features::GameLua::GetScanTotal());
    ImGui::Separator();

    // ── Memory-patch features (per-pattern) ────────────────────────────
    ImGui::TextDisabled("MEMORY PATCHES (need matching AOB for your build)");
    int count = 0;
    const auto* report = Features::Player::GetScanReport(count);
    if (ImGui::BeginTable("##scan", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableSetupColumn("Feature",  ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Status",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("Address",  ImGuiTableColumnFlags_WidthFixed, 130.0f);
        ImGui::TableHeadersRow();
        for (int i = 0; i < count; ++i) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(report[i].name);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(report[i].found ? ok : bad, report[i].found ? "FOUND" : "MISSING");
            ImGui::TableSetColumnIndex(2);
            if (report[i].found) ImGui::Text("0x%llX", (unsigned long long)report[i].addr);
            else                 ImGui::TextDisabled("-");
        }
        ImGui::EndTable();
    }
    ImGui::Separator();

    // ── Aggregate module scans ─────────────────────────────────────────
    ImGui::TextDisabled("OTHER SCANS");
    auto agg = [&](const char* n, int f, int t) {
        ImGui::Text("  %-14s", n); ImGui::SameLine();
        ImGui::TextColored(f == t ? ok : (f > 0 ? ImVec4(0.9f,0.8f,0.3f,1.0f) : bad),
                           "%d / %d", f, t);
    };
    agg("Resources", Features::Resources::GetScanFound(), Features::Resources::GetScanTotal());
    agg("World",     Features::World::GetScanFound(),     Features::World::GetScanTotal());
    agg("Visuals/ESP", Features::Visuals::GetScanFound(), Features::Visuals::GetScanTotal());
    ImGui::Separator();

    ImGui::TextWrapped("MISSING patterns do nothing until refreshed. In Cheat Engine: "
                       "find the value \xE2\x86\x92 'Find out what writes to this address' "
                       "\xE2\x86\x92 copy the instruction bytes \xE2\x86\x92 send them to be wired in.");
    ImGui::End();
}

void Render() {
    MenuSystem::Get().Render();

    if (Config::Get().luaConsoleOpen)
        Features::LuaConsole::RenderWindow();

    if (Config::Get().showScanReport)
        RenderScanReport();
}

} // namespace Menu
