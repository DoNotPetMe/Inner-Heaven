#include "tab_world.h"
#include "../../features/world.h"
#include <imgui.h>

namespace Tabs::World {

static const char* WeatherNames[] = { "Clear", "Cloudy", "Rainy", "Foggy", "Sandstorm" };

void Render() {
    if (!ImGui::BeginTabItem("World"))
        return;

    auto& s = Features::World::GetState();

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Time & Atmosphere");
    ImGui::Separator();

    ImGui::Checkbox("Slow Motion", &s.slowMotion);
    if (s.slowMotion) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::SliderFloat("##timescale", &s.timeScale, 0.05f, 0.9f, "%.2f");
    }

    ImGui::Checkbox("Override Time of Day", &s.overrideTime);
    if (s.overrideTime) {
        ImGui::SetNextItemWidth(200);
        ImGui::SliderFloat("Hour##tod", &s.timeOfDay, 0.0f, 24.0f, "%.1f h");

        ImGui::SameLine();
        if (ImGui::Button("Dawn"))  s.timeOfDay = 6.0f;
        ImGui::SameLine();
        if (ImGui::Button("Noon"))  s.timeOfDay = 12.0f;
        ImGui::SameLine();
        if (ImGui::Button("Dusk"))  s.timeOfDay = 18.0f;
        ImGui::SameLine();
        if (ImGui::Button("Night")) s.timeOfDay = 0.0f;
    }

    ImGui::Checkbox("Override Weather", &s.overrideWeather);
    if (s.overrideWeather) {
        int wi = static_cast<int>(s.weather);
        ImGui::SetNextItemWidth(200);
        if (ImGui::Combo("##weather", &wi, WeatherNames, IM_ARRAYSIZE(WeatherNames)))
            s.weather = static_cast<Features::World::Weather>(wi);
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Gameplay");
    ImGui::Separator();

    ImGui::Checkbox("No Enemy AI", &s.noEnemyAI);
    ImGui::SameLine(220); ImGui::TextDisabled("Enemies freeze in place");

    ImGui::Checkbox("Infinite Fulton", &s.infiniteFulton);
    ImGui::SameLine(220); ImGui::TextDisabled("Unlimited extractions");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Teleport");
    ImGui::Separator();

    if (ImGui::Button("Teleport to Waypoint"))
        s.teleportToWP = true;
    ImGui::SameLine();
    ImGui::TextDisabled("Place a marker on the map first");

    ImGui::EndTabItem();
}

} // namespace Tabs::World
