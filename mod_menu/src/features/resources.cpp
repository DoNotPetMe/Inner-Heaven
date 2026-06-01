#include "resources.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Resources {

static uintptr_t s_GMPAddr       = 0;
static uintptr_t s_HeroismAddr   = 0;
static uintptr_t s_DemonAddr     = 0;
static uintptr_t s_PlantsBase    = 0;
static uintptr_t s_MaterialsBase = 0;

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t g = Pattern::Scan("8B 81 ?? ?? ?? ?? 89 44 24 ?? 48 8B ?? ?? ?? ?? ?? E8", base, size);
    if (g) s_GMPAddr = g;

    uintptr_t h = Pattern::Scan("8B 81 ?? ?? ?? ?? 3B C2 7E ?? 89 91", base, size);
    if (h) s_HeroismAddr = h;

    uintptr_t d = Pattern::Scan("8B 81 ?? ?? ?? ?? 03 C2 89 81 ?? ?? ?? ?? 48", base, size);
    if (d) s_DemonAddr = d;

    uintptr_t p = Pattern::Scan("48 8D ?? ?? ?? ?? ?? 48 8B ?? ?? 4C 8D ?? ?? 48 8D ?? ?? ?? E8", base, size);
    if (p) s_PlantsBase = p;

    uintptr_t m = Pattern::Scan("48 8D ?? ?? ?? ?? ?? 44 8B ?? ?? ?? 48 8D ?? ?? E8 ?? ?? ?? ?? 48 8B", base, size);
    if (m) s_MaterialsBase = m;
}

int GetScanFound() {
    return (s_GMPAddr?1:0) + (s_HeroismAddr?1:0) + (s_DemonAddr?1:0)
         + (s_PlantsBase?1:0) + (s_MaterialsBase?1:0);
}
int GetScanTotal() { return 5; }

void SetGMP(int v)        { if (s_GMPAddr)     Memory::Write<int>(s_GMPAddr, v); }
void SetHeroism(int v)    { if (s_HeroismAddr) Memory::Write<int>(s_HeroismAddr, v); }
void SetDemonPoints(int v){ if (s_DemonAddr)   Memory::Write<int>(s_DemonAddr, v); }

void MaxAllPlants() {
    if (!s_PlantsBase) return;
    for (int i = 0; i < 20; ++i) Memory::Write<int>(s_PlantsBase + i * 4, 999);
}

void MaxAllMaterials() {
    if (!s_MaterialsBase) return;
    for (int i = 0; i < 12; ++i) Memory::Write<int>(s_MaterialsBase + i * 4, 999999);
}

void Tick() {
    auto& c = Config::Get();
    if (c.lockGMP        && s_GMPAddr)     Memory::Write<int>(s_GMPAddr, c.gmp);
    if (c.lockHeroism    && s_HeroismAddr) Memory::Write<int>(s_HeroismAddr, c.heroism);
    if (c.lockDemonPoints&& s_DemonAddr)   Memory::Write<int>(s_DemonAddr, c.demonPoints);
}

} // namespace Features::Resources
