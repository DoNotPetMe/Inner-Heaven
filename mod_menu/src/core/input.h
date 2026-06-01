#pragma once
#include <Windows.h>

namespace Input {

enum GamepadButton {
    PAD_DPAD_UP    = 0x0001,
    PAD_DPAD_DOWN  = 0x0002,
    PAD_DPAD_LEFT  = 0x0004,
    PAD_DPAD_RIGHT = 0x0008,
    PAD_START      = 0x0010,
    PAD_BACK       = 0x0020,
    PAD_LTHUMB     = 0x0040,
    PAD_RTHUMB     = 0x0080,
    PAD_LB         = 0x0100,
    PAD_RB         = 0x0200,
    PAD_A          = 0x1000,
    PAD_B          = 0x2000,
    PAD_X          = 0x4000,
    PAD_Y          = 0x8000,
};

void Init(HWND gameWindow);
void Shutdown();
void SetMenuOpen(bool open);
bool IsMenuOpen();
bool IsKeyPressed(int vKey);
bool IsKeyDown(int vKey);

void PollGamepad();
bool IsGamepadConnected();
bool GamepadPressed(int button);
bool GamepadRepeat(int button);
bool GamepadDown(int button);
void GetRightStick(float& outX, float& outY);

} // namespace Input
