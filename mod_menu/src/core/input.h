#pragma once
#include <Windows.h>

namespace Input {

void Init(HWND gameWindow);
void Shutdown();
bool IsMenuOpen();
bool IsKeyPressed(int vKey);
bool IsKeyDown(int vKey);

} // namespace Input
