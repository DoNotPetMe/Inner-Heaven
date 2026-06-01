#include "injection.h"

InjectionResult InjectDLL(DWORD pid, const std::string& dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
        return InjectionResult::ProcessOpenFailed;

    SIZE_T pathSize = dllPath.size() + 1;
    LPVOID remoteMem = VirtualAllocEx(hProcess, nullptr, pathSize,
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) {
        CloseHandle(hProcess);
        return InjectionResult::AllocFailed;
    }

    if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), pathSize, nullptr)) {
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return InjectionResult::WriteFailed;
    }

    FARPROC loadLibAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(
        hProcess, nullptr, 0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibAddr),
        remoteMem, 0, nullptr
    );

    if (!hThread) {
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return InjectionResult::ThreadFailed;
    }

    DWORD waitResult = WaitForSingleObject(hThread, 10000);

    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    if (waitResult == WAIT_TIMEOUT)
        return InjectionResult::Timeout;

    return InjectionResult::Success;
}

const char* InjectionResultToString(InjectionResult result) {
    switch (result) {
    case InjectionResult::Success:           return "Injection successful";
    case InjectionResult::ProcessOpenFailed: return "Failed to open process (run as Administrator)";
    case InjectionResult::AllocFailed:       return "Failed to allocate memory in target";
    case InjectionResult::WriteFailed:       return "Failed to write DLL path to target";
    case InjectionResult::ThreadFailed:      return "Failed to create remote thread";
    case InjectionResult::Timeout:           return "Remote thread timed out";
    default:                                 return "Unknown error";
    }
}
