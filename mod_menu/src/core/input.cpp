#include "input.h"
#include <imgui.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Input {

static HWND    s_GameWindow = nullptr;
static WNDPROC s_OrigWndProc = nullptr;
static bool    s_MenuOpen = false;
static bool    s_KeyStates[256] = {};
static bool    s_KeyPrev[256]   = {};

static LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_INSERT && !(lParam & 0x40000000)) {
        s_MenuOpen = !s_MenuOpen;

        ImGuiIO& io = ImGui::GetIO();
        if (s_MenuOpen) {
            io.MouseDrawCursor = true;
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        } else {
            io.MouseDrawCursor = false;
        }
    }

    if (msg == WM_KEYDOWN)
        s_KeyStates[wParam & 0xFF] = true;
    if (msg == WM_KEYUP)
        s_KeyStates[wParam & 0xFF] = false;

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
    bool pressed = s_KeyStates[vKey & 0xFF] && !s_KeyPrev[vKey & 0xFF];
    s_KeyPrev[vKey & 0xFF] = s_KeyStates[vKey & 0xFF];
    return pressed;
}

bool IsKeyDown(int vKey) {
    return s_KeyStates[vKey & 0xFF];
}

} // namespace Input
