#pragma once
#include <cstdint>

namespace Features::Resources {

void Init();
void Tick();

struct State {
    // Editable values
    int gmp          = 0;
    int heroism      = 0;
    int demonPoints  = 0;

    // Locks (freeze the value each frame)
    bool lockGMP         = false;
    bool lockHeroism     = false;
    bool lockDemonPoints = false;

    // Resource multiplier
    bool  resourceMultiplier = false;
    float multiplierValue    = 2.0f;

    // Materials
    bool maxPlants    = false;
    bool maxMaterials = false;
};

State& GetState();

// Immediate-set helpers (called from UI buttons)
void SetGMP(int value);
void SetHeroism(int value);
void SetDemonPoints(int value);
void MaxAllPlants();
void MaxAllMaterials();

} // namespace Features::Resources
