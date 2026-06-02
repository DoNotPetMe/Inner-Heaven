#pragma once

#include <string>

namespace Features::GameLua {

void Init();
void Tick();

void RunCode(const char* luaCode);
int RunCodeInt(const char* luaCode, int fallback);
// Synchronous read of a numeric Lua expression as a float (e.g. a coordinate).
// Runs on the calling thread; reserve for read-only probes.
float RunCodeFloat(const char* luaCode, float fallback);
// Synchronous read of a Lua expression as a string (read-only probes).
std::string RunCodeStr(const char* luaCode, const char* fallback = "");

// One-shot diagnostic: tests candidate gvars/mvars/vars fields + Tpp functions
// for the features that currently do nothing, and dumps the REAL matching keys
// from those tables. Results print to the Lua Console (persistent/scrollable).
void ProbeFields();

bool IsReady();

int GetScanFound();
int GetScanTotal();
void RunDiagnostics();

} // namespace Features::GameLua
