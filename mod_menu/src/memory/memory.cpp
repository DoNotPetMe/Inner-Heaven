#include "memory.h"
#include <cstring>

namespace Memory {

static uintptr_t s_BaseAddress = 0;
static size_t    s_ModuleSize  = 0;

void Init() {
    HMODULE hModule = GetModuleHandleA(nullptr);
    s_BaseAddress = reinterpret_cast<uintptr_t>(hModule);

    IMAGE_DOS_HEADER* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(s_BaseAddress);
    IMAGE_NT_HEADERS* nt  = reinterpret_cast<IMAGE_NT_HEADERS*>(s_BaseAddress + dos->e_lfanew);
    s_ModuleSize = nt->OptionalHeader.SizeOfImage;
}

uintptr_t GetBaseAddress() { return s_BaseAddress; }
size_t    GetModuleSize()  { return s_ModuleSize; }

void Patch(uintptr_t address, const uint8_t* bytes, size_t size) {
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect);
    std::memcpy(reinterpret_cast<void*>(address), bytes, size);
    VirtualProtect(reinterpret_cast<void*>(address), size, oldProtect, &oldProtect);
}

void Nop(uintptr_t address, size_t size) {
    DWORD oldProtect;
    VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &oldProtect);
    std::memset(reinterpret_cast<void*>(address), 0x90, size);
    VirtualProtect(reinterpret_cast<void*>(address), size, oldProtect, &oldProtect);
}

void PatchEntry::Apply() {
    if (address == 0) return;
    if (original.empty()) {
        original.resize(patched.size());
        std::memcpy(original.data(), reinterpret_cast<void*>(address), patched.size());
    }
    Patch(address, patched.data(), patched.size());
    active = true;
}

void PatchEntry::Restore() {
    if (address == 0 || original.empty()) return;
    Patch(address, original.data(), original.size());
    active = false;
}

void PatchEntry::Toggle() {
    if (active) Restore(); else Apply();
}

} // namespace Memory
