#include "world.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include "misc.h"

namespace Features::World {

static uintptr_t s_TimeScaleAddr = 0;
static uintptr_t s_TimeOfDayAddr = 0;
static float     s_OrigTimeScale = 1.0f;

static Memory::PatchEntry s_AIDisable;
static Memory::PatchEntry s_FultonPatch;

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t ts = Pattern::Scan("F3 0F 10 ?? ?? ?? ?? ?? F3 0F 59 ?? ?? F3 0F 11 ?? ?? ?? ?? ?? C3", base, size);
    if (ts) s_TimeScaleAddr = ts;

    uintptr_t tod = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? 0F 2F ?? ?? ?? ?? ?? 72 ?? F3 0F 10", base, size);
    if (tod) s_TimeOfDayAddr = tod;

    uintptr_t ai = Pattern::Scan("E8 ?? ?? ?? ?? 48 8B ?? ?? 48 85 C9 74 ?? E8 ?? ?? ?? ?? 84 C0 74", base, size);
    if (ai) { s_AIDisable.address = ai; s_AIDisable.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; }

    uintptr_t ful = Pattern::Scan("FF 88 ?? ?? ?? ?? 83 B8 ?? ?? ?? ?? 00 75", base, size);
    if (ful) { s_FultonPatch.address = ful; s_FultonPatch.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }; }
}

int GetScanFound() {
    return (s_TimeScaleAddr?1:0) + (s_TimeOfDayAddr?1:0)
         + (s_AIDisable.address?1:0) + (s_FultonPatch.address?1:0);
}
int GetScanTotal() { return 4; }

// Teleport to your saved marker (position slot 1). This goes through the
// verified Lua Warp (Misc::TeleportSlot) instead of the old AOB position-write,
// which never resolved on current builds. "Save Slot 1" in the Misc menu drops
// the marker; this warps you back to it.
void TeleportToWaypoint() {
    Features::Misc::TeleportSlot(1);
}

void Tick() {
    auto& c = Config::Get();

    // Slow motion
    static bool prevSlow = false;
    if (c.slowMotion != prevSlow) {
        if (s_TimeScaleAddr)
            Memory::Write<float>(s_TimeScaleAddr, c.slowMotion ? c.timeScale : s_OrigTimeScale);
        prevSlow = c.slowMotion;
    }
    if (c.slowMotion && s_TimeScaleAddr)
        Memory::Write<float>(s_TimeScaleAddr, c.timeScale);

    // Time override
    if (c.overrideTime && s_TimeOfDayAddr)
        Memory::Write<float>(s_TimeOfDayAddr, c.timeOfDay);

    // AI toggle
    static bool prevAI = false;
    if (c.noEnemyAI != prevAI) {
        c.noEnemyAI ? s_AIDisable.Apply() : s_AIDisable.Restore();
        prevAI = c.noEnemyAI;
    }

    // Fulton toggle
    static bool prevFul = false;
    if (c.infiniteFulton != prevFul) {
        c.infiniteFulton ? s_FultonPatch.Apply() : s_FultonPatch.Restore();
        prevFul = c.infiniteFulton;
    }
}

} // namespace Features::World
