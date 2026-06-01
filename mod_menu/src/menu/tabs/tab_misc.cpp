#include "tab_misc.h"
#include "../../features/misc.h"
#include <imgui.h>

namespace Tabs::Misc {

void Render() {
    if (!ImGui::BeginTabItem("Misc"))
        return;

    auto& s = Features::Misc::GetState();

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Position");
    ImGui::Separator();

    if (ImGui::Button("Save Position"))
        Features::Misc::SavePosition();
    ImGui::SameLine();
    if (ImGui::Button("Load Position"))
        Features::Misc::LoadPosition();
    ImGui::SameLine();
    if (s.hasSavedPos) {
        ImGui::TextDisabled("(%.1f, %.1f, %.1f)", s.savedX, s.savedY, s.savedZ);
    } else {
        ImGui::TextDisabled("No position saved");
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Camera");
    ImGui::Separator();

    ImGui::Checkbox("Free Camera", &s.freeCamera);
    if (s.freeCamera) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("Speed##cam", &s.camSpeed, 1.0f, 50.0f, "%.1f");
        ImGui::TextDisabled("WASD + Space/Ctrl to move");
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Fun");
    ImGui::Separator();

    ImGui::Checkbox("Custom Player Scale", &s.customScale);
    if (s.customScale) {
        ImGui::SetNextItemWidth(200);
        ImGui::SliderFloat("Scale##plscale", &s.scaleValue, 0.1f, 5.0f, "%.2f");
    }

    ImGui::Checkbox("Rapid Fire", &s.rapidFire);
    ImGui::SameLine(220); ImGui::TextDisabled("Remove fire rate limit");

    ImGui::Checkbox("No Fall Damage", &s.noFallDamage);
    ImGui::SameLine(220); ImGui::TextDisabled("Jump from any height");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextDisabled("Inner Heaven v1.0");
    ImGui::TextDisabled("[INSERT] Toggle Menu  |  [END] Eject Mod");

    ImGui::EndTabItem();
}

} // namespace Tabs::Misc
