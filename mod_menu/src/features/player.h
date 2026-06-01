#pragma once

namespace Features::Player {

void Init();
void Tick();

struct State {
    bool godMode          = false;
    bool infiniteAmmo     = false;
    bool noReload         = false;
    bool infiniteSuppressor = false;
    bool superSpeed       = false;
    bool stealthMode      = false;
    bool noRecoil         = false;
    bool oneHitKill       = false;
    bool infiniteReflex   = false;
    float speedMultiplier = 2.0f;
};

State& GetState();

} // namespace Features::Player
