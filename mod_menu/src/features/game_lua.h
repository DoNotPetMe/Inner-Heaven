#pragma once

namespace Features::GameLua {

void Init();
void Tick();

void RunCode(const char* luaCode);
int RunCodeInt(const char* luaCode, int fallback);
// Synchronous read of a numeric Lua expression as a float (e.g. a coordinate).
// Runs on the calling thread; reserve for read-only probes.
float RunCodeFloat(const char* luaCode, float fallback);

bool IsReady();

int GetScanFound();
int GetScanTotal();
void RunDiagnostics();

} // namespace Features::GameLua
