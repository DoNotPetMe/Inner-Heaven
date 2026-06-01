#pragma once
#include <cstdint>

namespace Features::Player {

void Init();
void Tick();

// One memory-patch feature's scan outcome, surfaced in the Debug report window.
struct ScanResult {
    const char* name;   // human-readable feature name
    bool        found;  // did the AOB pattern resolve on this build?
    uintptr_t   addr;   // resolved address (0 if not found)
};
constexpr int kMaxScan = 16;

int GetScanFound();
int GetScanTotal();
const ScanResult* GetScanReport(int& count);

} // namespace Features::Player
