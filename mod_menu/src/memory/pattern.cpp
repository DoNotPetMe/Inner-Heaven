#include "pattern.h"
#include "memory.h"
#include <vector>
#include <string>
#include <sstream>

namespace Pattern {

struct PatternByte {
    uint8_t value;
    bool    wildcard;
};

static std::vector<PatternByte> Parse(const char* pattern) {
    std::vector<PatternByte> bytes;
    std::istringstream stream(pattern);
    std::string token;

    while (stream >> token) {
        if (token == "??" || token == "?") {
            bytes.push_back({ 0, true });
        } else {
            bytes.push_back({ static_cast<uint8_t>(std::stoul(token, nullptr, 16)), false });
        }
    }
    return bytes;
}

uintptr_t Scan(const char* pattern, uintptr_t base, size_t size) {
    if (base == 0) base = Memory::GetBaseAddress();
    if (size == 0) size = Memory::GetModuleSize();

    auto parsed = Parse(pattern);
    if (parsed.empty()) return 0;

    const uint8_t* scanStart = reinterpret_cast<const uint8_t*>(base);
    const uint8_t* scanEnd   = scanStart + size - parsed.size();

    for (const uint8_t* current = scanStart; current <= scanEnd; ++current) {
        bool found = true;
        for (size_t i = 0; i < parsed.size(); ++i) {
            if (!parsed[i].wildcard && current[i] != parsed[i].value) {
                found = false;
                break;
            }
        }
        if (found)
            return reinterpret_cast<uintptr_t>(current);
    }

    return 0;
}

} // namespace Pattern
