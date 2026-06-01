#include "buddies.h"
#include "../config.h"
#include "../core/input.h"
#include "game_lua.h"
#include <imgui.h>
#include <windows.h>
#include <cmath>
#include <cstdio>

namespace Features::Buddies {

// ── Buddy + command tables ────────────────────────────────────────────────────
// Each buddy carries a short ring of commands. The command tokens (lower-case)
// are what we hand to the IHBuddy Lua helper; the labels are what the wheel shows.

enum BuddyId { B_DDOG = 0, B_QUIET, B_DHORSE, B_DWALKER, B_COUNT };

struct BuddyDef {
    const char* name;
    const char* luaId;
    const char* cmds[6];
    const char* cmdTokens[6];
    int         cmdCount;
};

static const BuddyDef s_Buddies[B_COUNT] = {
    { "D-DOG",    "ddog",
      { "Attack", "Scout", "Stay", "Come", "Mark", "Defend" },
      { "attack", "scout", "stay", "come", "mark", "defend" }, 6 },
    { "QUIET",    "quiet",
      { "Attack", "Scout", "Cover", "Stay", "Come", nullptr },
      { "attack", "scout", "cover", "stay", "come", nullptr }, 5 },
    { "D-HORSE",  "dhorse",
      { "Come", "Stay", "Follow", nullptr, nullptr, nullptr },
      { "come", "stay", "follow", nullptr, nullptr, nullptr }, 3 },
    { "D-WALKER", "dwalker",
      { "Attack", "Follow", "Guard", "Come", nullptr, nullptr },
      { "attack", "follow", "guard", "come", nullptr, nullptr }, 4 },
};

// ── The IHBuddy Lua helper ────────────────────────────────────────────────────
// Injected once. Two game-specific seams, both pcall-guarded so an unknown
// function is a silent no-op rather than a crash:
//   EnsureActive(list) — best-effort: ask the buddy manager to make each listed
//                        buddy present, so more than one can be out at a time.
//   Command(buddy,cmd) — route a single command token to one buddy.
// Every command also posts to the announce log, so the wheel gives the player
// visible feedback even on a build where the deeper calls are still being tuned.
static const char* kBuddyLua = R"LUA(
IHBuddy = IHBuddy or {}

function IHBuddy.EnsureActive(list)
  for _, b in ipairs(list) do
    pcall(function()
      if GkBuddy and GkBuddy.Appear then GkBuddy.Appear(b) end
    end)
    pcall(function()
      if TppBuddy2 and TppBuddy2.RequestAppear then TppBuddy2.RequestAppear(b) end
    end)
  end
end

function IHBuddy.Command(buddy, cmd)
  pcall(function()
    if GkBuddy and GkBuddy.Command then GkBuddy.Command(buddy, cmd) end
  end)
  pcall(function()
    if TppBuddyManager and TppBuddyManager.SendCommand then
      TppBuddyManager.SendCommand(buddy, cmd)
    end
  end)
  pcall(function()
    if TppUiCommand and TppUiCommand.AnnounceLogView then
      TppUiCommand.AnnounceLogView(string.upper(buddy) .. ": " .. string.upper(cmd))
    end
  end)
end
)LUA";

static bool s_HelperInjected = false;

// ── Wheel state ───────────────────────────────────────────────────────────────

static bool          s_WheelOpen        = false;
static bool          s_PrevHeld         = false;
static int           s_SelBuddy         = 0;            // index into the active list
static int           s_CmdIndex[B_COUNT] = { 0, 0, 0, 0 };
static ULONGLONG     s_LastEnsure       = 0;

static const float   kPi = 3.14159265358979323846f;

// Build the list of buddy ids the player has enabled for multi-buddy/wheel use.
static int CollectActive(int out[B_COUNT]) {
    auto& c = Config::Get();
    int n = 0;
    if (c.buddyDDogActive)    out[n++] = B_DDOG;
    if (c.buddyQuietActive)   out[n++] = B_QUIET;
    if (c.buddyDHorseActive)  out[n++] = B_DHORSE;
    if (c.buddyDWalkerActive) out[n++] = B_DWALKER;
    return n;
}

void Init() {
    s_HelperInjected = false;
}

void Tick() {
    auto& c = Config::Get();
    if (!c.multiBuddies) return;
    if (!GameLua::IsReady()) return;

    if (!s_HelperInjected) {
        GameLua::RunCode(kBuddyLua);
        s_HelperInjected = true;
    }

    // Periodically re-assert the active buddy set so they stay out together.
    ULONGLONG now = GetTickCount64();
    if (now - s_LastEnsure < 2500) return;
    s_LastEnsure = now;

    int act[B_COUNT];
    int n = CollectActive(act);
    if (n == 0) return;

    char list[160] = {0};
    char* p = list;
    p += snprintf(p, sizeof(list), "IHBuddy.EnsureActive({");
    for (int i = 0; i < n; ++i)
        p += snprintf(p, sizeof(list) - (p - list), "%s\"%s\"", i ? "," : "", s_Buddies[act[i]].luaId);
    snprintf(p, sizeof(list) - (p - list), "})");
    GameLua::RunCode(list);
}

bool IsWheelOpen() { return s_WheelOpen; }

static void IssueCommand(int buddyId) {
    if (!GameLua::IsReady()) return;
    if (!s_HelperInjected) {
        GameLua::RunCode(kBuddyLua);
        s_HelperInjected = true;
    }
    const BuddyDef& b = s_Buddies[buddyId];
    int ci = s_CmdIndex[buddyId];
    if (ci < 0 || ci >= b.cmdCount) ci = 0;
    char buf[128];
    snprintf(buf, sizeof(buf), "IHBuddy.Command(\"%s\",\"%s\")", b.luaId, b.cmdTokens[ci]);
    GameLua::RunCode(buf);
}

void RenderWheel() {
    auto& c = Config::Get();
    if (!c.buddyWheel) { s_WheelOpen = false; s_PrevHeld = false; return; }
    if (Input::IsMenuOpen()) { s_WheelOpen = false; s_PrevHeld = false; return; }

    // Hold-to-open: Y on the controller, X on the keyboard.
    bool held = false;
    if (c.buddyWheelGamepad  && Input::GamepadDown(Input::PAD_Y)) held = true;
    if (c.buddyWheelKeyboard && Input::IsKeyDown('X'))            held = true;

    int act[B_COUNT];
    int n = CollectActive(act);

    // Released this frame → fire the highlighted command, then close.
    if (s_PrevHeld && !held) {
        if (n > 0 && s_SelBuddy >= 0 && s_SelBuddy < n)
            IssueCommand(act[s_SelBuddy]);
        s_WheelOpen = false;
        s_PrevHeld  = false;
        return;
    }
    s_PrevHeld = held;
    if (!held) { s_WheelOpen = false; return; }

    if (!s_WheelOpen) { s_WheelOpen = true; s_SelBuddy = 0; } // fresh open

    // ── Selection input ───────────────────────────────────────────────────────
    if (n > 0) {
        // Controller: point the right stick at a buddy sector.
        float sx, sy;
        Input::GetRightStick(sx, sy);
        if (sx * sx + sy * sy > 0.36f) { // outside ~0.6 deadzone ring
            float ang  = atan2f(-sy, sx);          // screen-space (y down)
            float step = 2.0f * kPi / n;
            int k = (int)lroundf((ang + kPi * 0.5f) / step);
            k = ((k % n) + n) % n;
            s_SelBuddy = k;
        }
        // Keyboard: left/right cycles the highlighted buddy.
        if (Input::IsKeyPressed(VK_RIGHT)) s_SelBuddy = (s_SelBuddy + 1) % n;
        if (Input::IsKeyPressed(VK_LEFT))  s_SelBuddy = (s_SelBuddy - 1 + n) % n;

        // Cycle the highlighted buddy's command: bumpers or up/down.
        int bid = act[s_SelBuddy];
        int cc  = s_Buddies[bid].cmdCount;
        bool cmdNext = Input::GamepadPressed(Input::PAD_RB) || Input::IsKeyPressed(VK_DOWN);
        bool cmdPrev = Input::GamepadPressed(Input::PAD_LB) || Input::IsKeyPressed(VK_UP);
        if (cmdNext) s_CmdIndex[bid] = (s_CmdIndex[bid] + 1) % cc;
        if (cmdPrev) s_CmdIndex[bid] = (s_CmdIndex[bid] - 1 + cc) % cc;
    }

    // ── Draw ──────────────────────────────────────────────────────────────────
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    ImVec2 disp = ImGui::GetIO().DisplaySize;
    ImVec2 center(disp.x * 0.5f, disp.y * 0.5f);
    float scale = c.buddyWheelScale <= 0.0f ? 1.0f : c.buddyWheelScale;
    float R = 150.0f * scale;

    const ImU32 colRing   = IM_COL32(30, 35, 28, 200);
    const ImU32 colRingHi = IM_COL32(70, 110, 55, 230);
    const ImU32 colEdge   = IM_COL32(120, 170, 95, 255);
    const ImU32 colText   = IM_COL32(225, 230, 215, 255);
    const ImU32 colDim    = IM_COL32(150, 160, 140, 255);
    const ImU32 colAccent = IM_COL32(140, 210, 110, 255);

    dl->AddCircleFilled(center, R + 26.0f, IM_COL32(0, 0, 0, 120), 64);
    dl->AddCircle(center, R + 26.0f, colEdge, 64, 1.5f);

    if (n == 0) {
        const char* msg = "No active buddies";
        ImVec2 ts = ImGui::CalcTextSize(msg);
        dl->AddText(ImVec2(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f), colDim, msg);
        return;
    }

    float step = 2.0f * kPi / n;
    for (int i = 0; i < n; ++i) {
        float a = -kPi * 0.5f + step * i;
        ImVec2 p(center.x + cosf(a) * R, center.y + sinf(a) * R);
        bool sel = (i == s_SelBuddy);

        dl->AddCircleFilled(p, sel ? 40.0f * scale : 32.0f * scale, sel ? colRingHi : colRing, 32);
        dl->AddCircle(p, sel ? 40.0f * scale : 32.0f * scale, sel ? colEdge : colDim, 32, sel ? 2.0f : 1.0f);

        const char* name = s_Buddies[act[i]].name;
        ImVec2 ts = ImGui::CalcTextSize(name);
        dl->AddText(ImVec2(p.x - ts.x * 0.5f, p.y - ts.y * 0.5f), sel ? colText : colDim, name);
    }

    // Centre hub: selected buddy + its current command.
    int bid = act[s_SelBuddy];
    int ci  = s_CmdIndex[bid];
    if (ci < 0 || ci >= s_Buddies[bid].cmdCount) ci = 0;

    char hub[96];
    snprintf(hub, sizeof(hub), "%s", s_Buddies[bid].name);
    ImVec2 ht = ImGui::CalcTextSize(hub);
    dl->AddText(ImVec2(center.x - ht.x * 0.5f, center.y - ht.y - 2.0f), colText, hub);

    const char* cmd = s_Buddies[bid].cmds[ci];
    ImVec2 ct = ImGui::CalcTextSize(cmd);
    dl->AddText(ImVec2(center.x - ct.x * 0.5f, center.y + 4.0f), colAccent, cmd);

    // Hint line under the wheel.
    const char* hint = Input::IsGamepadConnected()
        ? "[R-Stick] Buddy   [LB/RB] Command   release [Y] to issue"
        : "[<-/->] Buddy   [Up/Dn] Command   release [X] to issue";
    ImVec2 hs = ImGui::CalcTextSize(hint);
    dl->AddText(ImVec2(center.x - hs.x * 0.5f, center.y + R + 34.0f), colDim, hint);
}

} // namespace Features::Buddies
