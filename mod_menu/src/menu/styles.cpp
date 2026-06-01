#include "styles.h"
#include <imgui.h>

namespace Styles {

void Apply() {
    ImGuiStyle& s = ImGui::GetStyle();

    // Match IH proportions: compact, clean, minimal rounding
    s.WindowRounding    = 2.0f;
    s.FrameRounding     = 1.0f;
    s.GrabRounding      = 1.0f;
    s.TabRounding       = 2.0f;
    s.ScrollbarRounding = 2.0f;
    s.ChildRounding     = 1.0f;
    s.PopupRounding     = 2.0f;

    s.WindowPadding  = ImVec2(6, 6);
    s.FramePadding   = ImVec2(4, 2);
    s.ItemSpacing    = ImVec2(4, 3);
    s.ScrollbarSize  = 12.0f;
    s.GrabMinSize    = 10.0f;
    s.IndentSpacing  = 12.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize  = 0.0f;

    // IH-inspired dark theme — close to ImGui's StyleColorsDark
    // with subtle adjustments for the military/tactical feel
    ImVec4* c = s.Colors;

    c[ImGuiCol_Text]                  = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    c[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_WindowBg]              = ImVec4(0.06f, 0.06f, 0.06f, 0.96f);
    c[ImGuiCol_ChildBg]               = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    c[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.96f);
    c[ImGuiCol_Border]                = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
    c[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_FrameBg]               = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    c[ImGuiCol_FrameBgHovered]        = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    c[ImGuiCol_FrameBgActive]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_TitleBg]               = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    c[ImGuiCol_TitleBgActive]         = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.04f, 0.04f, 0.04f, 0.60f);
    c[ImGuiCol_MenuBarBg]             = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    c[ImGuiCol_ScrollbarBg]           = ImVec4(0.04f, 0.04f, 0.04f, 0.60f);
    c[ImGuiCol_ScrollbarGrab]         = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    c[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    c[ImGuiCol_CheckMark]             = ImVec4(0.40f, 0.70f, 0.35f, 1.00f);
    c[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.60f, 0.30f, 1.00f);
    c[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 0.75f, 0.40f, 1.00f);
    c[ImGuiCol_Button]                = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    c[ImGuiCol_ButtonHovered]         = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    c[ImGuiCol_ButtonActive]          = ImVec4(0.30f, 0.50f, 0.28f, 1.00f);
    c[ImGuiCol_Header]                = ImVec4(0.18f, 0.30f, 0.16f, 0.80f);
    c[ImGuiCol_HeaderHovered]         = ImVec4(0.22f, 0.38f, 0.20f, 0.90f);
    c[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.46f, 0.24f, 1.00f);
    c[ImGuiCol_Separator]             = ImVec4(0.24f, 0.24f, 0.24f, 0.60f);
    c[ImGuiCol_SeparatorHovered]      = ImVec4(0.35f, 0.55f, 0.30f, 0.80f);
    c[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.65f, 0.35f, 1.00f);
    c[ImGuiCol_ResizeGrip]            = ImVec4(0.20f, 0.20f, 0.20f, 0.40f);
    c[ImGuiCol_ResizeGripHovered]     = ImVec4(0.30f, 0.50f, 0.28f, 0.60f);
    c[ImGuiCol_ResizeGripActive]      = ImVec4(0.35f, 0.60f, 0.30f, 0.90f);
    c[ImGuiCol_Tab]                   = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    c[ImGuiCol_TabHovered]            = ImVec4(0.22f, 0.38f, 0.20f, 0.90f);
    c[ImGuiCol_TabSelected]           = ImVec4(0.18f, 0.30f, 0.16f, 1.00f);
    c[ImGuiCol_TextSelectedBg]        = ImVec4(0.25f, 0.45f, 0.22f, 0.50f);
    c[ImGuiCol_NavCursor]             = ImVec4(0.35f, 0.60f, 0.30f, 1.00f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.10f, 0.10f, 0.10f, 0.60f);
}

} // namespace Styles
