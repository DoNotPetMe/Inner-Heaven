#pragma once
#include <Windows.h>
#include <string>

enum class InjectionResult {
    Success,
    ProcessOpenFailed,
    AllocFailed,
    WriteFailed,
    ThreadFailed,
    Timeout
};

InjectionResult InjectDLL(DWORD pid, const std::string& dllPath);
const char* InjectionResultToString(InjectionResult result);
