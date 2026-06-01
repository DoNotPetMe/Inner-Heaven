#include "player.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Player {

static Memory::PatchEntry s_GodMode;
static Memory::PatchEntry s_GodModeAlt;
static Memory::PatchEntry s_InfAmmo;
static Memory::PatchEntry s_NoReload;
static Memory::PatchEntry s_InfSuppressor;
static Memory::PatchEntry s_NoRecoil;
static Memory::PatchEntry s_OneHitKill;
static Memory::PatchEntry s_InfReflex;
static Memory::PatchEntry s_NoFallDamage;
static Memory::PatchEntry s_RapidFire;

static uintptr_t s_DetectionAddr = 0;

// ── Scan reporting ──────────────────────────────────────────────────────────
// Every memory-patch feature records whether its AOB pattern resolved on this
// game build. The Debug → Pattern Scan Report window reads this so you can see
// at a glance which patches are live vs. which need fresh patterns from Cheat
// Engine. `addr` is shown so a matched pattern can be cross-checked.
static ScanResult s_Report[kMaxScan];
static int        s_ReportCount = 0;
static int        s_ScanFound   = 0;

static void Record(const char* name, uintptr_t addr) {
    if (s_ReportCount < kMaxScan) {
        s_Report[s_ReportCount].name  = name;
        s_Report[s_ReportCount].found = (addr != 0);
        s_Report[s_ReportCount].addr  = addr;
        ++s_ReportCount;
    }
    if (addr) ++s_ScanFound;
}

void Init() {
    s_ScanFound   = 0;
    s_ReportCount = 0;
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // Community-verified CE patterns
    uintptr_t hp = Pattern::Scan("F3 0F 11 0E F3 45 0F 58 D8", base, size);
    if (hp) { s_GodMode.address = hp; s_GodMode.patched = { 0x90, 0x90, 0x90, 0x90 }; }
    Record("God Mode (HP write)", hp);

    uintptr_t hpAlt = Pattern::Scan("F3 41 0F 11 16 F3 44 0F 5C E6", base, size);
    if (hpAlt) { s_GodModeAlt.address = hpAlt; s_GodModeAlt.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; }
    Record("God Mode (alt)", hpAlt);

    uintptr_t ammo = Pattern::Scan("66 44 89 1C 48 49 8B 4A 58", base, size);
    if (ammo) { s_InfAmmo.address = ammo; s_InfAmmo.patched = { 0x90, 0x90, 0x90, 0x90, 0x90 }; }
    Record("Infinite Ammo", ammo);

    uintptr_t rel = Pattern::Scan("83 B9 ?? ?? ?? ?? 00 0F 8E ?? ?? ?? ?? 48 8B", base, size);
    if (rel) { s_NoReload.address = rel + 7; s_NoReload.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }; }
    Record("No Reload", rel);

    uintptr_t sup = Pattern::Scan("F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F", base, size);
    if (sup) { s_InfSuppressor.address = sup; s_InfSuppressor.patched.assign(16, 0x90); }
    Record("Infinite Suppressor", sup);

    uintptr_t rec = Pattern::Scan("F3 0F 58 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B", base, size);
    if (rec) { s_NoRecoil.address = rec; s_NoRecoil.patched.assign(16, 0x90); }
    Record("No Recoil", rec);

    uintptr_t ohk = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? 80 ?? ?? ?? ?? ?? 00 74", base, size);
    if (ohk) { s_OneHitKill.address = ohk; s_OneHitKill.patched = { 0x0F, 0x57, 0xC0, 0xF3, 0x0F, 0x11, 0x41, 0x00 }; }
    Record("One Hit Kill", ohk);

    uintptr_t rfx = Pattern::Scan("F3 0F 5C ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 0F 2F ?? ?? 76", base, size);
    if (rfx) { s_InfReflex.address = rfx; s_InfReflex.patched.assign(16, 0x90); }
    Record("Infinite Reflex", rfx);

    uintptr_t fall = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? EB ?? F3 0F 10 ?? ?? ?? ?? ?? 0F 2F C1 76 ?? F3 0F 5C", base, size);
    if (fall) { s_NoFallDamage.address = fall; s_NoFallDamage.patched.assign(8, 0x90); }
    Record("No Fall Damage", fall);

    uintptr_t fire = Pattern::Scan("0F 2F ?? ?? ?? ?? ?? 73 ?? F3 0F 58", base, size);
    if (fire) { s_RapidFire.address = fire; s_RapidFire.patched = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xEB }; }
    Record("Rapid Fire", fire);

    uintptr_t det = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8 ?? ?? ?? ?? 84 C0 74", base, size);
    if (det) { s_DetectionAddr = det; }
    Record("Stealth (detection)", det);
}

void Tick() {
    auto& c = Config::Get();

    auto toggle = [](bool on, bool& prev, Memory::PatchEntry& p) {
        if (on == prev) return;
        on ? p.Apply() : p.Restore();
        prev = on;
    };

    static bool p0=0,p1=0,p2=0,p3=0,p4=0,p5=0,p6=0,p7=0,p8=0,p9=0;
    toggle(c.godMode,           p0, s_GodMode);
    toggle(c.godMode,           p1, s_GodModeAlt);
    toggle(c.infiniteAmmo,      p2, s_InfAmmo);
    toggle(c.noReload,          p3, s_NoReload);
    toggle(c.infiniteSuppressor,p4, s_InfSuppressor);
    toggle(c.noRecoil,          p5, s_NoRecoil);
    toggle(c.oneHitKill,        p6, s_OneHitKill);
    toggle(c.infiniteReflex,    p7, s_InfReflex);
    toggle(c.noFallDamage,      p8, s_NoFallDamage);
    toggle(c.rapidFire,         p9, s_RapidFire);

    if (c.stealthMode && s_DetectionAddr)
        Memory::Write<float>(s_DetectionAddr, 0.0f);
}

int GetScanFound() { return s_ScanFound; }
int GetScanTotal() { return s_ReportCount; }

const ScanResult* GetScanReport(int& count) {
    count = s_ReportCount;
    return s_Report;
}

} // namespace Features::Player
