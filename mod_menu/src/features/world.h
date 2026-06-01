#pragma once

namespace Features::World {

void Init();
void Tick();

enum class Weather { Clear, Cloudy, Rainy, Foggy, Sandstorm };

struct State {
    bool  slowMotion     = false;
    float timeScale      = 0.3f;

    bool  overrideTime   = false;
    float timeOfDay      = 12.0f;

    bool    overrideWeather = false;
    Weather weather        = Weather::Clear;

    bool noEnemyAI       = false;
    bool teleportToWP    = false; // momentary trigger

    bool noGravity       = false;
    bool infiniteFulton  = false;
};

State& GetState();

void TeleportToWaypoint();

} // namespace Features::World
