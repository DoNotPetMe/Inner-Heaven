#pragma once

namespace Features::Misc {

void Init();
void Tick();

struct State {
    // Save/Load position
    bool hasSavedPos = false;
    float savedX = 0, savedY = 0, savedZ = 0;

    // Free camera
    bool freeCamera      = false;
    float camSpeed       = 5.0f;

    // Player scale
    bool  customScale    = false;
    float scaleValue     = 1.0f;

    // Infinite iDroid range
    bool infiniteIDroid   = false;

    // No fall damage
    bool noFallDamage    = false;

    // Rapid-fire
    bool rapidFire       = false;
};

State& GetState();

void SavePosition();
void LoadPosition();

} // namespace Features::Misc
