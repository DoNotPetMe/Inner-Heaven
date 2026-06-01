#include "resources.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"

namespace Features::Resources {

static State s_State;

static uintptr_t s_GMPAddr        = 0;
static uintptr_t s_HeroismAddr    = 0;
static uintptr_t s_DemonAddr      = 0;
static uintptr_t s_PlantsBase     = 0;
static uintptr_t s_MaterialsBase  = 0;

static constexpr int PLANT_SLOT_COUNT    = 20;
static constexpr int MATERIAL_SLOT_COUNT = 12;
static constexpr int RESOURCE_STRIDE     = 4; // int32 per slot

State& GetState() { return s_State; }

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // GMP read/write — mov eax, [reg+offset] where the offset corresponds to GMP
    uintptr_t gmpScan = Pattern::Scan(
        "8B 81 ?? ?? ?? ?? 89 44 24 ?? 48 8B ?? ?? ?? ?? ?? E8", base, size
    );
    if (gmpScan) {
        // Dereference the pointer chain: base + instruction operand gives the struct,
        // then the field offset gives GMP
        int32_t offset = Memory::Read<int32_t>(gmpScan + 2);
        // Store the scan result for now; the actual pointer resolution happens per-frame
        s_GMPAddr = gmpScan;
    }

    uintptr_t heroismScan = Pattern::Scan(
        "8B 81 ?? ?? ?? ?? 3B C2 7E ?? 89 91", base, size
    );
    if (heroismScan) {
        s_HeroismAddr = heroismScan;
    }

    uintptr_t demonScan = Pattern::Scan(
        "8B 81 ?? ?? ?? ?? 03 C2 89 81 ?? ?? ?? ?? 48", base, size
    );
    if (demonScan) {
        s_DemonAddr = demonScan;
    }

    // Plants/materials array base — accessed during resource collection
    uintptr_t plantsScan = Pattern::Scan(
        "48 8D ?? ?? ?? ?? ?? 48 8B ?? ?? 4C 8D ?? ?? 48 8D ?? ?? ?? E8", base, size
    );
    if (plantsScan) {
        s_PlantsBase = plantsScan;
    }

    uintptr_t matScan = Pattern::Scan(
        "48 8D ?? ?? ?? ?? ?? 44 8B ?? ?? ?? 48 8D ?? ?? E8 ?? ?? ?? ?? 48 8B", base, size
    );
    if (matScan) {
        s_MaterialsBase = matScan;
    }
}

void SetGMP(int value) {
    if (s_GMPAddr)
        Memory::Write<int>(s_GMPAddr, value);
}

void SetHeroism(int value) {
    if (s_HeroismAddr)
        Memory::Write<int>(s_HeroismAddr, value);
}

void SetDemonPoints(int value) {
    if (s_DemonAddr)
        Memory::Write<int>(s_DemonAddr, value);
}

void MaxAllPlants() {
    if (!s_PlantsBase) return;
    for (int i = 0; i < PLANT_SLOT_COUNT; ++i) {
        Memory::Write<int>(s_PlantsBase + i * RESOURCE_STRIDE, 999);
    }
}

void MaxAllMaterials() {
    if (!s_MaterialsBase) return;
    for (int i = 0; i < MATERIAL_SLOT_COUNT; ++i) {
        Memory::Write<int>(s_MaterialsBase + i * RESOURCE_STRIDE, 999999);
    }
}

void Tick() {
    if (s_State.lockGMP && s_GMPAddr)
        Memory::Write<int>(s_GMPAddr, s_State.gmp);

    if (s_State.lockHeroism && s_HeroismAddr)
        Memory::Write<int>(s_HeroismAddr, s_State.heroism);

    if (s_State.lockDemonPoints && s_DemonAddr)
        Memory::Write<int>(s_DemonAddr, s_State.demonPoints);
}

} // namespace Features::Resources
