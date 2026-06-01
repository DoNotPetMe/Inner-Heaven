#include "menu_system.h"
#include "../core/input.h"
#include "../config.h"
#include <imgui.h>
#include <cstdio>
#include <algorithm>

// ── Factory helpers ────────────────────────────────────────────────────────

MenuNode MakeSub(const char* name, const char* help, std::vector<MenuNode> ch) {
    MenuNode n{}; n.type = MenuNode::Sub; n.name = name; n.help = help;
    n.children = std::move(ch); return n;
}
MenuNode MakeToggle(const char* name, const char* help, bool* val) {
    MenuNode n{}; n.type = MenuNode::Toggle; n.name = name; n.help = help;
    n.boolVal = val; return n;
}
MenuNode MakeInt(const char* name, const char* help, int* val, int mn, int mx, int step, const char* sfx) {
    MenuNode n{}; n.type = MenuNode::Int; n.name = name; n.help = help;
    n.intVal = val; n.iMin = mn; n.iMax = mx; n.iStep = step; n.suffix = sfx; return n;
}
MenuNode MakeFloat(const char* name, const char* help, float* val, float mn, float mx, float step, const char* sfx) {
    MenuNode n{}; n.type = MenuNode::Float; n.name = name; n.help = help;
    n.floatVal = val; n.fMin = mn; n.fMax = mx; n.fStep = step; n.suffix = sfx; return n;
}
MenuNode MakeEnum(const char* name, const char* help, int* val, const char** names, int count) {
    MenuNode n{}; n.type = MenuNode::Enum; n.name = name; n.help = help;
    n.intVal = val; n.enumNames = names; n.enumCount = count; return n;
}
MenuNode MakeCmd(const char* name, const char* help, void(*func)()) {
    MenuNode n{}; n.type = MenuNode::Cmd; n.name = name; n.help = help;
    n.cmdFunc = func; return n;
}
MenuNode MakeSep() {
    MenuNode n{}; n.type = MenuNode::Sep; return n;
}

// ── MenuSystem ─────────────────────────────────────────────────────────────

MenuSystem& MenuSystem::Get() {
    static MenuSystem inst;
    return inst;
}

void MenuSystem::SetRoot(MenuNode root) {
    m_root = std::move(root);
    m_current = &m_root;
    m_selected = 0;
    m_stack.clear();
}

void MenuSystem::MoveSelection(int dir) {
    auto& items = m_current->children;
    int n = static_cast<int>(items.size());
    if (n == 0) return;

    int next = m_selected;
    for (int i = 0; i < n; ++i) {
        next = (next + dir + n) % n;
        if (items[next].type != MenuNode::Sep)
            break;
    }
    m_selected = next;
}

void MenuSystem::EnterSelected() {
    if (m_current->children.empty()) return;
    auto& item = m_current->children[m_selected];

    switch (item.type) {
    case MenuNode::Sub:
        m_stack.push_back({ m_current, m_selected });
        m_current = &m_current->children[m_selected];
        m_selected = 0;
        if (!m_current->children.empty() && m_current->children[0].type == MenuNode::Sep)
            MoveSelection(1);
        break;
    case MenuNode::Toggle:
        if (item.boolVal) *item.boolVal = !*item.boolVal;
        break;
    case MenuNode::Cmd:
        if (item.cmdFunc) item.cmdFunc();
        break;
    case MenuNode::Enum:
        ChangeValue(1);
        break;
    default:
        break;
    }
}

void MenuSystem::GoBack() {
    if (m_stack.empty()) return;
    auto& prev = m_stack.back();
    m_current  = prev.node;
    m_selected = prev.index;
    m_stack.pop_back();
}

void MenuSystem::ChangeValue(int dir) {
    if (m_current->children.empty()) return;
    auto& item = m_current->children[m_selected];

    switch (item.type) {
    case MenuNode::Toggle:
        if (item.boolVal) *item.boolVal = !*item.boolVal;
        break;
    case MenuNode::Int:
        if (item.intVal) {
            *item.intVal += dir * item.iStep;
            if (*item.intVal < item.iMin) *item.intVal = item.iMin;
            if (*item.intVal > item.iMax) *item.intVal = item.iMax;
        }
        break;
    case MenuNode::Float:
        if (item.floatVal) {
            *item.floatVal += dir * item.fStep;
            if (*item.floatVal < item.fMin) *item.floatVal = item.fMin;
            if (*item.floatVal > item.fMax) *item.floatVal = item.fMax;
        }
        break;
    case MenuNode::Enum:
        if (item.intVal) {
            *item.intVal += dir;
            if (*item.intVal < 0) *item.intVal = item.enumCount - 1;
            if (*item.intVal >= item.enumCount) *item.intVal = 0;
        }
        break;
    default:
        break;
    }
}

void MenuSystem::HandleInput() {
    bool kbActive = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    bool up    = (kbActive && ImGui::IsKeyPressed(ImGuiKey_UpArrow))    || Input::GamepadRepeat(Input::PAD_DPAD_UP);
    bool down  = (kbActive && ImGui::IsKeyPressed(ImGuiKey_DownArrow))  || Input::GamepadRepeat(Input::PAD_DPAD_DOWN);
    bool right = (kbActive && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) || Input::GamepadRepeat(Input::PAD_DPAD_RIGHT);
    bool left  = (kbActive && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))  || Input::GamepadRepeat(Input::PAD_DPAD_LEFT);

    bool enter = (kbActive && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)))
               || Input::GamepadPressed(Input::PAD_A);

    bool back  = (kbActive && (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Backspace)))
               || Input::GamepadPressed(Input::PAD_B);

    bool home  = (kbActive && ImGui::IsKeyPressed(ImGuiKey_Home)) || Input::GamepadPressed(Input::PAD_LB);
    bool end   = (kbActive && ImGui::IsKeyPressed(ImGuiKey_End))  || Input::GamepadPressed(Input::PAD_RB);

    if (up)    MoveSelection(-1);
    if (down)  MoveSelection(1);
    if (right) ChangeValue(1);
    if (left)  ChangeValue(-1);
    if (enter) EnterSelected();

    if (back) {
        if (m_stack.empty())
            Input::SetMenuOpen(false);
        else
            GoBack();
    }

    if (home) { m_selected = 0; MoveSelection(0); }
    if (end) {
        m_selected = static_cast<int>(m_current->children.size()) - 1;
        if (m_selected >= 0 && m_current->children[m_selected].type == MenuNode::Sep)
            MoveSelection(-1);
    }
}

void MenuSystem::DrawBreadcrumb() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.75f, 0.35f, 1.0f));
    for (auto& entry : m_stack) {
        ImGui::TextUnformatted(entry.node->name);
        ImGui::SameLine(0, 2);
        ImGui::TextUnformatted(">");
        ImGui::SameLine(0, 2);
    }
    ImGui::TextUnformatted(m_current->name);
    ImGui::PopStyleColor();
}

void MenuSystem::DrawItem(const MenuNode& item, int index) {
    if (item.type == MenuNode::Sep) {
        ImGui::Separator();
        return;
    }

    bool selected = (index == m_selected);
    char buf[256];

    switch (item.type) {
    case MenuNode::Sub:
        snprintf(buf, sizeof(buf), "  > %s", item.name);
        break;
    case MenuNode::Toggle:
        snprintf(buf, sizeof(buf), "  %-36s%s", item.name,
                 (item.boolVal && *item.boolVal) ? "ON" : "OFF");
        break;
    case MenuNode::Int:
        snprintf(buf, sizeof(buf), "  %-36s%d%s", item.name,
                 item.intVal ? *item.intVal : 0, item.suffix);
        break;
    case MenuNode::Float:
        snprintf(buf, sizeof(buf), "  %-36s%.2f%s", item.name,
                 item.floatVal ? *item.floatVal : 0.0f, item.suffix);
        break;
    case MenuNode::Enum: {
        const char* valName = "???";
        if (item.intVal && item.enumNames && *item.intVal >= 0 && *item.intVal < item.enumCount)
            valName = item.enumNames[*item.intVal];
        snprintf(buf, sizeof(buf), "  %-36s%s", item.name, valName);
        break;
    }
    case MenuNode::Cmd:
        snprintf(buf, sizeof(buf), "  >> %s", item.name);
        break;
    default:
        return;
    }

    // Color the value portion for toggles
    if (item.type == MenuNode::Toggle && item.boolVal && *item.boolVal) {
        // Draw selectable with colored ON
        ImGui::PushStyleColor(ImGuiCol_Text, selected
            ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
            : ImGui::GetStyleColorVec4(ImGuiCol_Text));

        char namePart[256];
        snprintf(namePart, sizeof(namePart), "  %-36s", item.name);

        if (ImGui::Selectable(namePart, selected, 0, ImVec2(0, 0))) {
            m_selected = index;
            EnterSelected();
        }
        ImGui::SameLine(0, 0);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.85f, 0.3f, 1.0f));
        ImGui::TextUnformatted("ON");
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        return;
    }

    if (item.type == MenuNode::Sub) {
        // Highlight submenu arrows
        ImGui::PushStyleColor(ImGuiCol_Text, selected
            ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
            : ImGui::GetStyleColorVec4(ImGuiCol_Text));

        char label[256];
        snprintf(label, sizeof(label), "  > %s##%d", item.name, index);
        if (ImGui::Selectable(label, selected)) {
            m_selected = index;
            EnterSelected();
        }
        ImGui::PopStyleColor();
        return;
    }

    if (item.type == MenuNode::Cmd) {
        ImGui::PushStyleColor(ImGuiCol_Text, selected
            ? ImVec4(1.0f, 1.0f, 0.7f, 1.0f)
            : ImVec4(0.9f, 0.85f, 0.5f, 1.0f));
    }

    char label[256];
    snprintf(label, sizeof(label), "%s##%d", buf, index);
    if (ImGui::Selectable(label, selected)) {
        m_selected = index;
        EnterSelected();
    }

    if (item.type == MenuNode::Cmd)
        ImGui::PopStyleColor();
}

void MenuSystem::Render() {
    if (!m_open || !m_current) return;

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380, 540), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar
                           | ImGuiWindowFlags_NoScrollWithMouse
                           | ImGuiWindowFlags_NoCollapse
                           | ImGuiWindowFlags_NoSavedSettings;

    if (!ImGui::Begin("Inner Heaven##menu", &m_open, flags)) {
        ImGui::End();
        return;
    }

    // Breadcrumb path
    DrawBreadcrumb();
    ImGui::Separator();

    // Calculate layout
    float helpHeight = Config::Get().showHelp ? 70.0f : 0.0f;
    float navHint    = 20.0f;
    float listHeight = ImGui::GetContentRegionAvail().y - helpHeight - navHint - 8.0f;

    // Main item list
    auto& items = m_current->children;
    int count = static_cast<int>(items.size());

    if (ImGui::BeginListBox("##items", ImVec2(-1, listHeight))) {
        for (int i = 0; i < count; ++i)
            DrawItem(items[i], i);
        ImGui::EndListBox();
    }

    // Navigation hint
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.52f, 0.42f, 0.7f));
    if (Input::IsGamepadConnected())
        ImGui::TextUnformatted("[D-Pad] Navigate  [A] Select  [B] Back  [LB+RB] Close");
    else
        ImGui::TextUnformatted("[Arrows] Navigate  [Enter] Select  [Esc] Back  [INS] Close");
    ImGui::PopStyleColor();

    // Help text
    if (Config::Get().showHelp && m_selected >= 0 && m_selected < count
        && items[m_selected].type != MenuNode::Sep)
    {
        ImGui::Separator();
        ImGui::BeginChild("##help", ImVec2(-1, 0), false);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.75f, 0.65f, 1.0f));
        ImGui::TextWrapped("%s", items[m_selected].help);
        ImGui::PopStyleColor();
        ImGui::EndChild();
    }

    HandleInput();
    ImGui::End();
}
