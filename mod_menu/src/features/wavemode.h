#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Wave Survival — Inner Heaven's own stealth-survival gamemode.
//
// A self-contained "side job" arena: you drop into a location, enemies spawn in
// a ring around you, and an on-screen counter (top-left) shows how many remain.
// Thin them out — stealthily or loud — and when the counter hits zero a new,
// larger wave reinforces in. Inspired by Splinter Cell: Blacklist's wave maps.
//
// Reinforcements arrive NEUTRAL: freshly-spawned soldiers stay passive until you
// are actually spotted, at which point the whole arena turns hostile. This keeps
// each wave fair to open with a stealth approach before it escalates.
//
// The state machine, scoring and HUD live here in C++. The two game-specific
// touch points — spawning soldiers and reading whether the player is detected —
// are delegated to a small injected Lua helper (IHWave.*) so they can be tuned
// per game version without touching the gamemode logic.
// ─────────────────────────────────────────────────────────────────────────────

namespace Features::WaveMode {

void Init();        // resolves Lua bridge, injects the IHWave helper
void Tick();        // advances the wave state machine (call once per frame)
void RenderHud();   // draws the counter / status overlay (call once per frame)

void Start();       // begin a session using the current Config wave settings
void Stop();        // tear down the active session and clear spawned enemies

// Verified probe: true if the player's CURRENT area has a reinforcement block
// (i.e. the engine can actually spawn a wave here). The menu shows this so you
// know whether to start a session where you're standing.
bool CanSpawnHere();

// Announce in-game whether the current spot can host a wave (location scouting).
void CheckLocation();

bool IsRunning();
int  CurrentWave();
int  EnemiesRemaining();
int  Score();

} // namespace Features::WaveMode
