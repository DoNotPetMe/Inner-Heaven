#include "tab_player.h"
#include "../../features/player.h"
#include <imgui.h>

namespace Tabs::Player {

void Render() {
    if (!ImGui::BeginTabItem("Player"))
        return;

    auto& s = Features::Player::GetState();

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Combat");
    ImGui::Separator();

    ImGui::Checkbox("God Mode",            &s.godMode);
    ImGui::SameLine(220); ImGui::TextDisabled("Take no damage");

    ImGui::Checkbox("Infinite Ammo",       &s.infiniteAmmo);
    ImGui::SameLine(220); ImGui::TextDisabled("Never run out");

    ImGui::Checkbox("No Reload",           &s.noReload);
    ImGui::SameLine(220); ImGui::TextDisabled("Skip magazine changes");

    ImGui::Checkbox("Infinite Suppressors", &s.infiniteSuppressor);
    ImGui::SameLine(220); ImGui::TextDisabled("Suppressors never wear");

    ImGui::Checkbox("No Recoil",           &s.noRecoil);
    ImGui::SameLine(220); ImGui::TextDisabled("Perfectly stable aim");

    ImGui::Checkbox("One Hit Kill",        &s.oneHitKill);
    ImGui::SameLine(220); ImGui::TextDisabled("Enemies die instantly");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Movement & Stealth");
    ImGui::Separator();

    ImGui::Checkbox("Stealth Mode",        &s.stealthMode);
    ImGui::SameLine(220); ImGui::TextDisabled("Never detected");

    ImGui::Checkbox("Infinite Reflex",     &s.infiniteReflex);
    ImGui::SameLine(220); ImGui::TextDisabled("Reflex mode lasts forever");

    ImGui::Checkbox("Super Speed",         &s.superSpeed);
    if (s.superSpeed) {
        ImGui::SameLine(220);
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##speedmul", &s.speedMultiplier, 1.5f, 10.0f, "x%.1f");
    }

    ImGui::EndTabItem();
}

} // namespace Tabs::Player
