#pragma once

namespace Features::Visuals {

void Init();
void RenderOverlay();
int GetScanFound();
int GetScanTotal();

// Counts living enemy entities (hp above threshold) within `radius` metres of
// the given world-space centre. Used by the Wave Survival gamemode to drive its
// on-screen enemy counter from real game state. Returns -1 if the entity list
// could not be read this frame.
int CountEnemiesInRadius(float cx, float cy, float cz, float radius);

} // namespace Features::Visuals
