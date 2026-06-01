#include "wavemode.h"
#include "../config.h"
#include "game_lua.h"
#include "visuals.h"
#include "misc.h"
#include <imgui.h>
#include <windows.h>
#include <cstdio>
#include <cmath>
#include <cfloat>

namespace Features::WaveMode {

// ── Arena themes ─────────────────────────────────────────────────────────────
// The arena is always built around the player's current location, so any spot in
// the world can become a battleground. The theme only flavours the encounter
// (weather + a label) — it does not relocate the player.
struct ArenaTheme { const char* name; int weather; }; // weather index matches Config s_Weather
static const ArenaTheme s_Arenas[] = {
    { "AFGHAN OUTPOST",   1 }, // SUNNY
    { "AFRICA VILLAGE",   2 }, // CLOUDY
    { "MOTHER BASE DECK", 0 }, // keep current
    { "RUINS COMPOUND",   5 }, // FOGGY
    { "AIRPORT FACILITY", 3 }, // RAINY
    { "CURRENT LOCATION", 0 }, // keep current
};
static const int kArenaCount = (int)(sizeof(s_Arenas) / sizeof(s_Arenas[0]));

// ── The IHWave Lua helper ────────────────────────────────────────────────────
// Injected once into the game's Lua state. This is the single game-specific seam:
// SpawnWave / SetHostile / Clear wrap the Fox Engine soldier-spawn API (guarded
// with pcall so a missing function never crashes the game), and IsDetected
// returns whether the player is currently being seen. The GkEnemy/TppSequence
// calls below are the documented tuning points per game build.
static const char* kWaveLua = R"LUA(
IHWave = IHWave or { units = {}, hostile = false }

function IHWave._spawnOne(x, y, z, hostile, diff)
  pcall(function()
    if GkEnemy and GkEnemy.SpawnSoldier then
      local id = GkEnemy.SpawnSoldier({ pos = { x, y, z }, hostile = hostile, level = diff })
      if id then IHWave.units[#IHWave.units + 1] = id end
    end
  end)
end

function IHWave.SpawnWave(n, x, y, z, radius, diff, hostile)
  IHWave.hostile = hostile
  local twoPi = 6.2831853
  for i = 1, n do
    local a  = twoPi * (i / n)
    local sx = x + math.cos(a) * radius
    local sz = z + math.sin(a) * radius
    IHWave._spawnOne(sx, y, sz, hostile, diff)
  end
  pcall(function()
    if TppUiCommand and TppUiCommand.AnnounceLogView then
      TppUiCommand.AnnounceLogView("WAVE INCOMING")
    end
  end)
end

function IHWave.SetHostile()
  IHWave.hostile = true
  for _, id in ipairs(IHWave.units) do
    pcall(function()
      if GkEnemy and GkEnemy.SetCombat then GkEnemy.SetCombat(id, true) end
    end)
  end
end

function IHWave.Clear()
  for _, id in ipairs(IHWave.units) do
    pcall(function()
      if GkEnemy and GkEnemy.Remove then GkEnemy.Remove(id) end
    end)
  end
  IHWave.units   = {}
  IHWave.hostile = false
end

-- Returns 1 if the player is currently detected/under alert, else 0.
function IHWave.IsDetected()
  local lvl = 0
  pcall(function()
    if TppSequence and TppSequence.IsActiveDangerMusic and TppSequence.IsActiveDangerMusic() then
      lvl = 1
    end
  end)
  pcall(function()
    if TppEnemyManager and TppEnemyManager.GetAlertLevel then
      local a = TppEnemyManager.GetAlertLevel()
      if a and a > 0 then lvl = 1 end
    end
  end)
  return lvl
end
)LUA";

// ── Session state ────────────────────────────────────────────────────────────
enum class State { Inactive, Intro, Active, Cleared, Victory };

static State     s_State          = State::Inactive;
static bool      s_HelperInjected = false;

static float     s_CenterX = 0, s_CenterY = 0, s_CenterZ = 0;
static int       s_Wave            = 0;
static int       s_Remaining       = 0;
static int       s_PeakThisWave    = 0;
static int       s_Score           = 0;
static bool      s_HostileNow      = false;   // arena has gone loud
static bool      s_DetectedThisWave= false;   // were we ever spotted this wave
static int       s_KillsTotal      = 0;

static ULONGLONG s_WaveStartMs     = 0;
static ULONGLONG s_ClearedMs       = 0;
static ULONGLONG s_VictoryMs       = 0;
static ULONGLONG s_LastDetectPoll  = 0;

// Timing constants (ms)
static const ULONGLONG kSpawnGrace   = 1800;  // wait before a 0-count can clear a wave
static const ULONGLONG kSpawnTimeout = 9000;  // if nothing ever spawned, advance anyway
static const ULONGLONG kDetectPoll   = 250;   // how often to query detection
static const ULONGLONG kVictoryHold  = 9000;  // how long the victory banner lingers

// ── Helpers ──────────────────────────────────────────────────────────────────
static int EnemyCountForWave(int wave) {
    auto& c = Config::Get();
    int n = c.waveStartEnemies + (wave - 1) * c.waveEnemyIncrement;
    if (n < 1) n = 1;
    if (n > c.waveMaxEnemies) n = c.waveMaxEnemies;
    return n;
}

static void SpawnWave(int wave) {
    auto& c = Config::Get();
    int n = EnemyCountForWave(wave);
    bool hostile = !c.waveNeutralUntilSpotted;

    char buf[256];
    snprintf(buf, sizeof(buf),
             "IHWave.SpawnWave(%d, %.3f, %.3f, %.3f, %d, %d, %s)",
             n, s_CenterX, s_CenterY, s_CenterZ,
             c.waveSpawnRadius, c.waveDifficulty, hostile ? "true" : "false");
    GameLua::RunCode(buf);

    s_PeakThisWave     = 0;
    s_Remaining        = n;
    s_HostileNow       = hostile;
    s_DetectedThisWave = hostile;
    s_WaveStartMs      = GetTickCount64();
    s_State            = State::Intro;
}

static void AwardWaveBonus() {
    auto& c = Config::Get();
    int bonus = 100 * s_Wave * (c.waveDifficulty + 1);
    if (!s_DetectedThisWave && c.waveAutoStealthBonus)
        bonus += bonus / 2;                 // ghost bonus
    if (s_DetectedThisWave && c.waveLethalPenalty)
        bonus -= bonus / 4;                 // went loud
    if (bonus < 0) bonus = 0;
    s_Score      += bonus;
    s_KillsTotal += s_PeakThisWave;
}

static void GoHostile() {
    if (s_HostileNow) return;
    s_HostileNow       = true;
    s_DetectedThisWave = true;
    GameLua::RunCode("IHWave.SetHostile()");
}

// ── Public API ───────────────────────────────────────────────────────────────
void Init() {
    // Inject the helper as soon as the Lua bridge is alive. If it isn't ready
    // yet (early frames), Start() will retry the injection.
    if (GameLua::IsReady() && !s_HelperInjected) {
        GameLua::RunCode(kWaveLua);
        s_HelperInjected = true;
    }
}

void Start() {
    auto& c = Config::Get();
    c.waveModeActiveFailed = false;

    auto pos = Features::Misc::GetPlayerPos();
    if (!pos) {
        // Can't anchor an arena without knowing where the player is.
        c.waveModeActiveFailed = true;
        return;
    }
    s_CenterX = pos[0];
    s_CenterY = pos[1];
    s_CenterZ = pos[2];

    if (!s_HelperInjected && GameLua::IsReady()) {
        GameLua::RunCode(kWaveLua);
        s_HelperInjected = true;
    }

    // Clean slate.
    GameLua::RunCode("IHWave.Clear()");

    if (c.waveForceWeather && s_Arenas[c.waveArena % kArenaCount].weather > 0) {
        static const char* wx[] = {
            "", "SUNNY", "CLOUDY", "RAINY", "SANDSTORM", "FOGGY", "POURING",
        };
        char wbuf[128];
        snprintf(wbuf, sizeof(wbuf),
                 "TppWeather.SetCurrentWeather(TppDefine.WEATHER.%s)",
                 wx[s_Arenas[c.waveArena % kArenaCount].weather]);
        GameLua::RunCode(wbuf);
    }

    s_Wave        = 1;
    s_Score       = 0;
    s_KillsTotal  = 0;
    SpawnWave(s_Wave);
}

void Stop() {
    GameLua::RunCode("IHWave.Clear()");
    s_State       = State::Inactive;
    s_Remaining   = 0;
    s_HostileNow  = false;
}

void Tick() {
    auto& c = Config::Get();

    // Keep the public "active" flag in sync so the menu can reflect state.
    c.waveModeActive = (s_State != State::Inactive);

    if (s_State == State::Inactive)
        return;

    ULONGLONG now = GetTickCount64();

    // Poll detection on a timer; flip the arena hostile the moment we're seen.
    if (c.waveNeutralUntilSpotted && !s_HostileNow &&
        (s_State == State::Active || s_State == State::Intro) &&
        now - s_LastDetectPoll >= kDetectPoll) {
        s_LastDetectPoll = now;
        if (GameLua::RunCodeInt("return IHWave.IsDetected()", 0) != 0)
            GoHostile();
    }

    switch (s_State) {
    case State::Intro: {
        // Let freshly-spawned soldiers settle into the entity list before we let
        // a zero-count count as "cleared".
        if (now - s_WaveStartMs >= kSpawnGrace)
            s_State = State::Active;
        break;
    }
    case State::Active: {
        int alive = Features::Visuals::CountEnemiesInRadius(
            s_CenterX, s_CenterY, s_CenterZ, (float)c.waveSpawnRadius + 25.0f);

        if (alive >= 0) {
            s_Remaining = alive;
            if (alive > s_PeakThisWave) s_PeakThisWave = alive;

            if (alive == 0 && s_PeakThisWave > 0) {
                // Wave down.
                AwardWaveBonus();
                if (c.waveMaxWaves > 0 && s_Wave >= c.waveMaxWaves) {
                    s_State     = State::Victory;
                    s_VictoryMs = now;
                } else {
                    s_State     = State::Cleared;
                    s_ClearedMs = now;
                }
            } else if (s_PeakThisWave == 0 && now - s_WaveStartMs >= kSpawnTimeout) {
                // Spawning never registered (spawn API needs tuning on this build)
                // — advance rather than soft-lock the session.
                s_State     = State::Cleared;
                s_ClearedMs = now;
            }
        }
        break;
    }
    case State::Cleared: {
        if (now - s_ClearedMs >= (ULONGLONG)c.waveReinforceDelay * 1000) {
            ++s_Wave;
            SpawnWave(s_Wave);
        }
        break;
    }
    case State::Victory: {
        if (now - s_VictoryMs >= kVictoryHold)
            Stop();
        break;
    }
    default: break;
    }
}

// ── HUD ──────────────────────────────────────────────────────────────────────
static void DrawPanel(ImDrawList* dl, ImVec2 p, ImVec2 size) {
    dl->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), IM_COL32(8, 10, 12, 200), 4.0f);
    dl->AddRect      (p, ImVec2(p.x + size.x, p.y + size.y), IM_COL32(90, 160, 90, 220), 4.0f, 0, 1.5f);
}

void RenderHud() {
    auto& c = Config::Get();
    if (s_State == State::Inactive || !c.waveShowHud)
        return;

    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImFont* font   = ImGui::GetFont();
    float sw       = ImGui::GetIO().DisplaySize.x;

    const float px = 24.0f, py = 24.0f;
    const float pw = 240.0f, ph = 118.0f;
    DrawPanel(dl, ImVec2(px, py), ImVec2(pw, ph));

    float x = px + 14.0f;
    float y = py + 10.0f;

    dl->AddText(font, 14.0f, ImVec2(x, y), IM_COL32(120, 200, 120, 255), "WAVE SURVIVAL");
    dl->AddText(font, 12.0f, ImVec2(x + pw - 78.0f, y + 2.0f), IM_COL32(150, 150, 150, 220),
                s_Arenas[c.waveArena % kArenaCount].name);
    y += 22.0f;

    char line[96];
    if (c.waveMaxWaves > 0) snprintf(line, sizeof(line), "WAVE  %d / %d", s_Wave, c.waveMaxWaves);
    else                    snprintf(line, sizeof(line), "WAVE  %d  (ENDLESS)", s_Wave);
    dl->AddText(font, 18.0f, ImVec2(x, y), IM_COL32(235, 235, 235, 255), line);
    y += 26.0f;

    // Big hostiles counter.
    char num[16]; snprintf(num, sizeof(num), "%d", s_Remaining < 0 ? 0 : s_Remaining);
    dl->AddText(font, 40.0f, ImVec2(x, y - 4.0f), IM_COL32(255, 80, 80, 255), num);
    dl->AddText(font, 13.0f, ImVec2(x + 64.0f, y + 14.0f), IM_COL32(200, 200, 200, 230), "HOSTILES");
    y += 44.0f;

    // Status line.
    if (c.waveNeutralUntilSpotted) {
        if (s_HostileNow)
            dl->AddText(font, 14.0f, ImVec2(x, y), IM_COL32(255, 70, 70, 255),  "[!] DETECTED");
        else
            dl->AddText(font, 14.0f, ImVec2(x, y), IM_COL32(80, 220, 110, 255), "[-] UNDETECTED");
    }
    char sc[32]; snprintf(sc, sizeof(sc), "SCORE %d", s_Score);
    dl->AddText(font, 14.0f, ImVec2(x + pw - 110.0f, y), IM_COL32(220, 210, 120, 255), sc);

    // Centre banners for between-wave / victory states.
    auto centerBanner = [&](const char* big, ImU32 col, const char* sub) {
        ImVec2 ts = font->CalcTextSizeA(34.0f, FLT_MAX, 0.0f, big);
        float cx = sw * 0.5f - ts.x * 0.5f;
        dl->AddText(font, 34.0f, ImVec2(cx, 90.0f), col, big);
        if (sub) {
            ImVec2 ss = font->CalcTextSizeA(16.0f, FLT_MAX, 0.0f, sub);
            dl->AddText(font, 16.0f, ImVec2(sw * 0.5f - ss.x * 0.5f, 128.0f),
                        IM_COL32(220, 220, 220, 230), sub);
        }
    };

    if (s_State == State::Cleared) {
        ULONGLONG left = (ULONGLONG)c.waveReinforceDelay * 1000;
        ULONGLONG el   = GetTickCount64() - s_ClearedMs;
        int secs = (int)((left > el ? left - el : 0) / 1000) + 1;
        char sub[64]; snprintf(sub, sizeof(sub), "REINFORCEMENTS IN %d...", secs);
        centerBanner("WAVE CLEARED", IM_COL32(90, 220, 120, 255), sub);
    } else if (s_State == State::Victory) {
        char sub[64]; snprintf(sub, sizeof(sub), "FINAL SCORE  %d", s_Score);
        centerBanner("ARENA CLEARED", IM_COL32(255, 215, 90, 255), sub);
    }
}

bool IsRunning()       { return s_State != State::Inactive; }
int  CurrentWave()     { return s_Wave; }
int  EnemiesRemaining(){ return s_Remaining; }
int  Score()           { return s_Score; }

} // namespace Features::WaveMode
