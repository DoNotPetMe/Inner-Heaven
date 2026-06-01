#include "menu.h"
#include "tabs/tab_player.h"
#include "tabs/tab_resources.h"
#include "tabs/tab_world.h"
#include "tabs/tab_visuals.h"
#include "tabs/tab_misc.h"
#include <imgui.h>

namespace Menu {

static void DrawHeader() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f, 0.55f, 0.24f, 1.0f));
    ImGui::TextUnformatted("INNER HEAVEN");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::TextDisabled("MGSV:TPP");
    ImGui::SameLine(ImGui::GetWindowWidth() - 160);
    ImGui::TextDisabled("[INSERT] Toggle Menu");
    ImGui::Separator();
}

void Render() {
    ImGui::SetNextWindowSize(ImVec2(680, 520), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Inner Heaven##main", nullptr, flags);

    DrawHeader();

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_FittingPolicyResizeDown)) {
        Tabs::Player::Render();
        Tabs::Resources::Render();
        Tabs::World::Render();
        Tabs::Visuals::Render();
        Tabs::Misc::Render();
        ImGui::EndTabBar();
    }

    ImGui::End();
}

} // namespace Menu
