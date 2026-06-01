#include "input.h"
#include "../menu/menu_system.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <Xinput.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace Input {

static HWND    s_GameWindow  = nullptr;
static WNDPROC s_OrigWndProc = nullptr;
static bool    s_MenuOpen    = false;

// ── Gamepad state ─────────────────────────────────────────────────────────────

static XINPUT_STATE s_PadCur{};
static XINPUT_STATE s_PadPrev{};
static bool  s_PadConnected = false;
static WORD  s_ButtonEdge   = 0;
static WORD  s_VirtualDpad  = 0;
static WORD  s_PrevVirtualDpad = 0;

static const SHORT kStickDeadZone  = 16000;
static const ULONGLONG kRepeatDelay = 400;
static const ULONGLONG kRepeatRate  = 100;

struct RepeatTimer {
    ULONGLONG firstDown = 0;
    ULONGLONG lastFire  = 0;
    bool prevDown       = false;
};

static RepeatTimer s_DpadRepeat[4];
static bool s_DpadFired[4] = {};

static const WORD kDirBits[4] = {
    XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT
};

static WORD ComputeVirtualDpad(const XINPUT_GAMEPAD& pad) {
    WORD v = pad.wButtons & (XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN |
                              XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT);
    if (pad.sThumbLY >  kStickDeadZone) v |= XINPUT_GAMEPAD_DPAD_UP;
    if (pad.sThumbLY < -kStickDeadZone) v |= XINPUT_GAMEPAD_DPAD_DOWN;
    if (pad.sThumbLX < -kStickDeadZone) v |= XINPUT_GAMEPAD_DPAD_LEFT;
    if (pad.sThumbLX >  kStickDeadZone) v |= XINPUT_GAMEPAD_DPAD_RIGHT;
    return v;
}

// ── Menu open/close ───────────────────────────────────────────────────────────

void SetMenuOpen(bool open) {
    s_MenuOpen = open;
    MenuSystem::Get().SetOpen(open);
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = open;
    if (!open)
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    else
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

// ── WndProc hook ──────────────────────────────────────────────────────────────

static LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && !(lParam & 0x40000000)) {
        if (wParam == VK_INSERT || wParam == VK_F3)
            SetMenuOpen(!s_MenuOpen);
    }

    if (s_MenuOpen) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            return 0;
    }

    return CallWindowProcA(s_OrigWndProc, hwnd, msg, wParam, lParam);
}

// ── Public API ────────────────────────────────────────────────────────────────

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

// ── Gamepad polling ───────────────────────────────────────────────────────────

void PollGamepad() {
    s_PadPrev = s_PadCur;
    s_PrevVirtualDpad = s_VirtualDpad;

    DWORD result = XInputGetState(0, &s_PadCur);
    s_PadConnected = (result == ERROR_SUCCESS);

    if (!s_PadConnected) {
        s_VirtualDpad = 0;
        s_ButtonEdge = 0;
        for (int i = 0; i < 4; ++i) s_DpadFired[i] = false;
        return;
    }

    s_VirtualDpad = ComputeVirtualDpad(s_PadCur.Gamepad);
    s_ButtonEdge = (s_PadCur.Gamepad.wButtons ^ s_PadPrev.Gamepad.wButtons)
                 & s_PadCur.Gamepad.wButtons;

    ULONGLONG now = GetTickCount64();
    for (int i = 0; i < 4; ++i) {
        bool down = (s_VirtualDpad & kDirBits[i]) != 0;
        bool prev = s_DpadRepeat[i].prevDown;
        s_DpadFired[i] = false;

        if (down && !prev) {
            s_DpadRepeat[i].firstDown = now;
            s_DpadRepeat[i].lastFire = now;
            s_DpadFired[i] = true;
        } else if (down) {
            ULONGLONG held = now - s_DpadRepeat[i].firstDown;
            if (held >= kRepeatDelay && now - s_DpadRepeat[i].lastFire >= kRepeatRate) {
                s_DpadRepeat[i].lastFire = now;
                s_DpadFired[i] = true;
            }
        }
        s_DpadRepeat[i].prevDown = down;
    }

    // LB + RB combo toggles menu
    WORD cur = s_PadCur.Gamepad.wButtons;
    WORD prev = s_PadPrev.Gamepad.wButtons;
    bool combo = (cur & XINPUT_GAMEPAD_LEFT_SHOULDER) && (cur & XINPUT_GAMEPAD_RIGHT_SHOULDER);
    bool prevCombo = (prev & XINPUT_GAMEPAD_LEFT_SHOULDER) && (prev & XINPUT_GAMEPAD_RIGHT_SHOULDER);

    if (combo && !prevCombo)
        SetMenuOpen(!s_MenuOpen);
}

bool IsGamepadConnected() { return s_PadConnected; }

bool GamepadPressed(int button) {
    if (!s_PadConnected) return false;
    return (s_ButtonEdge & static_cast<WORD>(button)) != 0;
}

bool GamepadRepeat(int button) {
    if (!s_PadConnected) return false;
    for (int i = 0; i < 4; ++i) {
        if (kDirBits[i] == static_cast<WORD>(button))
            return s_DpadFired[i];
    }
    return GamepadPressed(button);
}

} // namespace Input
