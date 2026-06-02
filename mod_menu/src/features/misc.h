#pragma once

namespace Features::Misc {

void Init();
void Tick();
void SavePosition();   // slot 1 (back-compat)
void LoadPosition();   // slot 1 (back-compat)

// Verified Lua-backed teleport. slot is 1-3.
void SaveSlot(int slot);
void TeleportSlot(int slot);

// Warp the player to an explicit world position (Lua Warp). rotYDeg in degrees.
void WarpTo(float x, float y, float z, float rotYDeg);

// Read the live player position+rotation from the game. out = {x,y,z,rotYDeg}.
// Returns false if the Lua bridge isn't ready / the read failed.
bool ReadPlayerPos(float out[4]);

// Returns a pointer to a static XYZ buffer refreshed from the live Lua read,
// or nullptr if unavailable. (Back-compat accessor.)
float* GetPlayerPosPtr();

struct PosRef {
    float* p;
    operator bool() const { return p != nullptr; }
    float& operator[](int i) { return p[i]; }
};
PosRef GetPlayerPos();

} // namespace Features::Misc
