#include "input.h"
#include "../menu/menu_system.h"
#include <imgui.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Input {

static HWND    s_GameWindow  = nullptr;
static WNDPROC s_OrigWndProc = nullptr;
static bool    s_MenuOpen    = false;

static LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // INSERT toggles menu (matching IH's F3, but INSERT is our default)
    if (msg == WM_KEYDOWN && !(lParam & 0x40000000)) {
        if (wParam == VK_INSERT || wParam == VK_F3) {
            s_MenuOpen = !s_MenuOpen;
            MenuSystem::Get().SetOpen(s_MenuOpen);

            ImGuiIO& io = ImGui::GetIO();
            io.MouseDrawCursor = s_MenuOpen;
            if (!s_MenuOpen)
                io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            else
                io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        }
    }

    if (s_MenuOpen) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            return 0;
    }

    return CallWindowProcA(s_OrigWndProc, hwnd, msg, wParam, lParam);
}

void Init(HWND gameWindow) {
    s_GameWindow = gameWindow;
    s_OrigWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(gameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HookWndProc))
    );
}

void Shutdown() {
    if (s_GameWindow && s_OrigWndProc) {
        SetWindowLongPtrA(s_GameWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_OrigWndProc));
        s_OrigWndProc = nullptr;
    }
}

bool IsMenuOpen() { return s_MenuOpen; }

bool IsKeyPressed(int vKey) {
    return (GetAsyncKeyState(vKey) & 1) != 0;
}

bool IsKeyDown(int vKey) {
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}

} // namespace Input
