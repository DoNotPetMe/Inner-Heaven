#pragma once
#include <cstdint>

namespace Pattern {

// Pattern format: "48 8B 05 ?? ?? ?? ?? 48 85 C0" where ?? is a wildcard byte
uintptr_t Scan(const char* pattern, uintptr_t base = 0, size_t size = 0);

} // namespace Pattern
