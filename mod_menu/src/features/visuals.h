#pragma once

namespace Features::Visuals {

void Init();
void RenderOverlay();

struct State {
    bool  espEnabled     = false;
    bool  espBoxes       = true;
    bool  espDistance     = true;
    bool  espHealthBar   = false;

    bool  nightVision    = false;
    float nightVisionStr = 2.0f;

    bool  customFOV      = false;
    float fovValue       = 75.0f;

    bool  crosshair      = false;
    int   crosshairSize  = 8;
};

State& GetState();

} // namespace Features::Visuals
