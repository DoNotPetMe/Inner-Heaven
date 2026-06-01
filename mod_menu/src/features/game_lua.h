#pragma once

namespace Features::GameLua {

void Init();
void Tick();

void RunCode(const char* luaCode);
int RunCodeInt(const char* luaCode, int fallback);

bool IsReady();

int GetScanFound();
int GetScanTotal();
void RunDiagnostics();

} // namespace Features::GameLua
