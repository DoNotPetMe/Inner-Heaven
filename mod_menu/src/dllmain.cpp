#include "core/hooks.h"
#include "memory/memory.h"
#include <Windows.h>

static HMODULE g_Module = nullptr;

static DWORD WINAPI InitThread(LPVOID hModule) {
    // Brief delay to let the game and other overlays finish initializing
    Sleep(2000);

    Memory::Init();
    Hooks::Init();

    // Wait for eject key (END)
    while (!(GetAsyncKeyState(VK_END) & 1))
        Sleep(100);

    Hooks::Shutdown();
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        g_Module = hModule;
        HANDLE hThread = CreateThread(nullptr, 0, InitThread, hModule, 0, nullptr);
        if (hThread)
            CloseHandle(hThread);
    }
    return TRUE;
}
