#include "tab_resources.h"
#include "../../features/resources.h"
#include <imgui.h>

namespace Tabs::Resources {

void Render() {
    if (!ImGui::BeginTabItem("Resources"))
        return;

    auto& s = Features::Resources::GetState();

    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Economy");
    ImGui::Separator();

    // GMP
    ImGui::SetNextItemWidth(180);
    ImGui::InputInt("GMP##edit", &s.gmp);
    ImGui::SameLine();
    if (ImGui::Button("Set##gmp"))
        Features::Resources::SetGMP(s.gmp);
    ImGui::SameLine();
    ImGui::Checkbox("Lock##gmplock", &s.lockGMP);

    // Heroism
    ImGui::SetNextItemWidth(180);
    ImGui::InputInt("Heroism##edit", &s.heroism);
    ImGui::SameLine();
    if (ImGui::Button("Set##heroism"))
        Features::Resources::SetHeroism(s.heroism);
    ImGui::SameLine();
    ImGui::Checkbox("Lock##herlock", &s.lockHeroism);

    // Demon Points
    ImGui::SetNextItemWidth(180);
    ImGui::InputInt("Demon Points##edit", &s.demonPoints);
    ImGui::SameLine();
    if (ImGui::Button("Set##demon"))
        Features::Resources::SetDemonPoints(s.demonPoints);
    ImGui::SameLine();
    ImGui::Checkbox("Lock##demlock", &s.lockDemonPoints);

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Presets");
    ImGui::Separator();

    if (ImGui::Button("Max GMP (5M)")) {
        s.gmp = 5000000;
        Features::Resources::SetGMP(s.gmp);
    }
    ImGui::SameLine();
    if (ImGui::Button("Max Heroism (1M)")) {
        s.heroism = 1000000;
        Features::Resources::SetHeroism(s.heroism);
    }
    ImGui::SameLine();
    if (ImGui::Button("Zero Demon Pts")) {
        s.demonPoints = 0;
        Features::Resources::SetDemonPoints(s.demonPoints);
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.30f, 0.55f, 0.24f, 1.0f), "Inventory");
    ImGui::Separator();

    if (ImGui::Button("Max All Plants"))
        Features::Resources::MaxAllPlants();
    ImGui::SameLine();
    if (ImGui::Button("Max All Materials"))
        Features::Resources::MaxAllMaterials();

    ImGui::Spacing();
    ImGui::Checkbox("Resource Multiplier", &s.resourceMultiplier);
    if (s.resourceMultiplier) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::SliderFloat("##resmul", &s.multiplierValue, 1.0f, 10.0f, "x%.1f");
    }

    ImGui::EndTabItem();
}

} // namespace Tabs::Resources
