#pragma once
#include <Windows.h>
#include <string>
#include <optional>

struct ProcessInfo {
    DWORD pid;
    std::string name;
};

std::optional<ProcessInfo> FindProcessByName(const std::string& processName);
