#pragma once

namespace Features::LuaConsole {

void Init();
void RenderWindow();

// Append a line to the console log from elsewhere (e.g. the field probe).
// kind: 0 = normal/out, 1 = system, 2 = error, 3 = good/found.
void PrintLine(const char* text, int kind);

// Force the console window open.
void Open();

} // namespace Features::LuaConsole
