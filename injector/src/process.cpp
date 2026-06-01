#include "process.h"
#include <TlHelp32.h>
#include <algorithm>

std::optional<ProcessInfo> FindProcessByName(const std::string& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return std::nullopt;

    PROCESSENTRY32A entry{};
    entry.dwSize = sizeof(entry);

    if (!Process32FirstA(snapshot, &entry)) {
        CloseHandle(snapshot);
        return std::nullopt;
    }

    std::string targetLower = processName;
    std::transform(targetLower.begin(), targetLower.end(), targetLower.begin(), ::tolower);

    do {
        std::string current(entry.szExeFile);
        std::transform(current.begin(), current.end(), current.begin(), ::tolower);

        if (current == targetLower) {
            CloseHandle(snapshot);
            return ProcessInfo{ entry.th32ProcessID, std::string(entry.szExeFile) };
        }
    } while (Process32NextA(snapshot, &entry));

    CloseHandle(snapshot);
    return std::nullopt;
}
