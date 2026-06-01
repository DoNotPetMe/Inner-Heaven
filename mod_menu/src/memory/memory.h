#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

namespace Memory {

void Init();

uintptr_t GetBaseAddress();
size_t    GetModuleSize();

template<typename T>
T Read(uintptr_t address) {
    return *reinterpret_cast<T*>(address);
}

template<typename T>
void Write(uintptr_t address, T value) {
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
    *reinterpret_cast<T*>(address) = value;
    VirtualProtect(reinterpret_cast<void*>(address), sizeof(T), oldProtect, &oldProtect);
}

void Patch(uintptr_t address, const uint8_t* bytes, size_t size);
void Nop(uintptr_t address, size_t size);

struct PatchEntry {
    uintptr_t address;
    std::vector<uint8_t> original;
    std::vector<uint8_t> patched;
    bool active = false;

    void Apply();
    void Restore();
    void Toggle();
};

} // namespace Memory
