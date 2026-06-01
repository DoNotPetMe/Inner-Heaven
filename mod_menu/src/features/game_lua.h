#pragma once

namespace Features::GameLua {

void Init();
void Tick();

void RunCode(const char* luaCode);

// Runs Lua that returns a single value, parsed as an integer. Returns `fallback`
// if the bridge is unavailable or the chunk errors. Used for state queries
// (e.g. "is the player currently detected?").
int RunCodeInt(const char* luaCode, int fallback);

bool IsReady();

} // namespace Features::GameLua
