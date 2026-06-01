#include "menu.h"
#include "menu_system.h"
#include "menu_defs.h"
#include "../features/lua_console.h"
#include "../config.h"
#include <imgui.h>

namespace Menu {

void Init() {
    MenuSystem::Get().SetRoot(BuildMenuTree());
}

void Render() {
    MenuSystem::Get().Render();

    if (Config::Get().luaConsoleOpen)
        Features::LuaConsole::RenderWindow();

    if (Config::Get().showPosition) {
        ImGui::SetNextWindowPos(ImVec2(10, 560), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 40), ImGuiCond_FirstUseEver);
        ImGui::Begin("##pos", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
        ImGui::TextDisabled("Position display requires pattern scan");
        ImGui::End();
    }
}

} // namespace Menu
