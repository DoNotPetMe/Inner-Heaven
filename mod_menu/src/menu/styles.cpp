#include "styles.h"
#include <imgui.h>

namespace Styles {

void Apply() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding    = 6.0f;
    s.FrameRounding     = 3.0f;
    s.GrabRounding      = 3.0f;
    s.TabRounding       = 4.0f;
    s.ScrollbarRounding = 4.0f;
    s.ChildRounding     = 4.0f;
    s.PopupRounding     = 4.0f;

    s.WindowPadding  = ImVec2(12, 12);
    s.FramePadding   = ImVec2(8, 4);
    s.ItemSpacing    = ImVec2(8, 6);
    s.ScrollbarSize  = 14.0f;
    s.GrabMinSize    = 12.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize  = 0.0f;

    ImVec4* c = s.Colors;

    // MGS-inspired dark military green theme
    ImVec4 bg       = ImVec4(0.04f, 0.05f, 0.04f, 0.95f);
    ImVec4 bgChild  = ImVec4(0.06f, 0.08f, 0.06f, 1.00f);
    ImVec4 frame    = ImVec4(0.10f, 0.14f, 0.09f, 1.00f);
    ImVec4 frameHov = ImVec4(0.15f, 0.22f, 0.13f, 1.00f);
    ImVec4 accent   = ImVec4(0.22f, 0.42f, 0.18f, 1.00f);
    ImVec4 accentHi = ImVec4(0.30f, 0.55f, 0.24f, 1.00f);
    ImVec4 text     = ImVec4(0.82f, 0.88f, 0.78f, 1.00f);
    ImVec4 textDim  = ImVec4(0.50f, 0.58f, 0.46f, 1.00f);
    ImVec4 border   = ImVec4(0.16f, 0.22f, 0.14f, 0.80f);
    ImVec4 title    = ImVec4(0.08f, 0.12f, 0.07f, 1.00f);
    ImVec4 titleAct = ImVec4(0.12f, 0.20f, 0.10f, 1.00f);

    c[ImGuiCol_Text]                  = text;
    c[ImGuiCol_TextDisabled]          = textDim;
    c[ImGuiCol_WindowBg]              = bg;
    c[ImGuiCol_ChildBg]               = bgChild;
    c[ImGuiCol_PopupBg]               = ImVec4(0.05f, 0.07f, 0.05f, 0.96f);
    c[ImGuiCol_Border]                = border;
    c[ImGuiCol_BorderShadow]          = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg]               = frame;
    c[ImGuiCol_FrameBgHovered]        = frameHov;
    c[ImGuiCol_FrameBgActive]         = accent;
    c[ImGuiCol_TitleBg]               = title;
    c[ImGuiCol_TitleBgActive]         = titleAct;
    c[ImGuiCol_TitleBgCollapsed]      = title;
    c[ImGuiCol_MenuBarBg]             = bgChild;
    c[ImGuiCol_ScrollbarBg]           = ImVec4(0.03f, 0.04f, 0.03f, 0.60f);
    c[ImGuiCol_ScrollbarGrab]         = accent;
    c[ImGuiCol_ScrollbarGrabHovered]  = accentHi;
    c[ImGuiCol_ScrollbarGrabActive]   = accentHi;
    c[ImGuiCol_CheckMark]             = accentHi;
    c[ImGuiCol_SliderGrab]            = accent;
    c[ImGuiCol_SliderGrabActive]      = accentHi;
    c[ImGuiCol_Button]                = frame;
    c[ImGuiCol_ButtonHovered]         = frameHov;
    c[ImGuiCol_ButtonActive]          = accent;
    c[ImGuiCol_Header]                = frame;
    c[ImGuiCol_HeaderHovered]         = frameHov;
    c[ImGuiCol_HeaderActive]          = accent;
    c[ImGuiCol_Separator]             = border;
    c[ImGuiCol_SeparatorHovered]      = accent;
    c[ImGuiCol_SeparatorActive]       = accentHi;
    c[ImGuiCol_ResizeGrip]            = frame;
    c[ImGuiCol_ResizeGripHovered]     = accent;
    c[ImGuiCol_ResizeGripActive]      = accentHi;
    c[ImGuiCol_Tab]                   = frame;
    c[ImGuiCol_TabHovered]            = accent;
    c[ImGuiCol_TabSelected]           = titleAct;
    c[ImGuiCol_TabSelectedOverline]   = accentHi;
    c[ImGuiCol_TabDimmed]             = title;
    c[ImGuiCol_TabDimmedSelected]     = frame;
    c[ImGuiCol_PlotLines]             = accent;
    c[ImGuiCol_PlotLinesHovered]      = accentHi;
    c[ImGuiCol_PlotHistogram]         = accent;
    c[ImGuiCol_PlotHistogramHovered]  = accentHi;
    c[ImGuiCol_TableHeaderBg]         = title;
    c[ImGuiCol_TableBorderStrong]     = border;
    c[ImGuiCol_TableBorderLight]      = ImVec4(border.x, border.y, border.z, 0.40f);
    c[ImGuiCol_TableRowBg]            = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.02f);
    c[ImGuiCol_TextSelectedBg]        = ImVec4(accent.x, accent.y, accent.z, 0.40f);
    c[ImGuiCol_DragDropTarget]        = accentHi;
    c[ImGuiCol_NavCursor]             = accentHi;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.2f, 0.2f, 0.2f, 0.20f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.1f, 0.1f, 0.1f, 0.60f);
}

} // namespace Styles
