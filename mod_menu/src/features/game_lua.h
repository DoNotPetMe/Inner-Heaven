#pragma once

namespace Features::GameLua {

void Init();
void Tick();

void RunCode(const char* luaCode);

} // namespace Features::GameLua
