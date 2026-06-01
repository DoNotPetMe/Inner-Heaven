#pragma once

namespace Features::Misc {

void Init();
void Tick();
void SavePosition();
void LoadPosition();

// Returns a writable pointer to the player XYZ floats (3 consecutive floats), or nullptr
float* GetPlayerPosPtr();

struct PosRef {
    float* p;
    operator bool() const { return p != nullptr; }
    float& operator[](int i) { return p[i]; }
};
PosRef GetPlayerPos();

} // namespace Features::Misc
