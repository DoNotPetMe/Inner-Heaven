#include "tab_visuals.h"
#include "../../features/visuals.h"
#include <imgui.h>

namespace Tabs::Visuals {

void Render() {
    if (!ImGui::BeginTabItem("Visuals"))
        return;

    auto& s = Features::Visuals::GetState();

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "ESP");
    ImGui::Separator();

    ImGui::Checkbox("Enable ESP", &s.espEnabled);

    if (s.espEnabled) {
        ImGui::Indent(16);
        ImGui::Checkbox("Bounding Boxes", &s.espBoxes);
        ImGui::Checkbox("Distance",       &s.espDistance);
        ImGui::Checkbox("Health Bars",    &s.espHealthBar);
        ImGui::Unindent(16);
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Camera");
    ImGui::Separator();

    ImGui::Checkbox("Custom FOV", &s.customFOV);
    if (s.customFOV) {
        ImGui::SetNextItemWidth(200);
        ImGui::SliderFloat("FOV##fovslider", &s.fovValue, 40.0f, 140.0f, "%.0f deg");
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Rendering");
    ImGui::Separator();

    ImGui::Checkbox("Night Vision", &s.nightVision);
    if (s.nightVision) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##nv_str", &s.nightVisionStr, 1.2f, 5.0f, "%.1f");
    }

    ImGui::Checkbox("Crosshair Overlay", &s.crosshair);
    if (s.crosshair) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::SliderInt("Size##xhair", &s.crosshairSize, 4, 24);
    }

    ImGui::EndTabItem();
}

} // namespace Tabs::Visuals
