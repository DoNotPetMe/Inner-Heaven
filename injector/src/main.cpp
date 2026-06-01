#include "console.h"
#include "process.h"
#include "injection.h"
#include <filesystem>

static std::string GetDllPath() {
    char buf[MAX_PATH]{};
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::filesystem::path exePath(buf);
    return (exePath.parent_path() / "InnerHeavenMenu.dll").string();
}

int main() {
    SetConsoleTitleA("Inner Heaven - MGSV:TPP Injector");
    Console::PrintBanner();

    std::string dllPath = GetDllPath();
    if (!std::filesystem::exists(dllPath)) {
        Console::Print("InnerHeavenMenu.dll not found next to injector!", Console::Status::Error);
        Console::Print("Expected: " + dllPath, Console::Status::Info);
        std::cout << "\n Press any key to exit...";
        std::cin.get();
        return 1;
    }

    Console::Print("DLL: " + dllPath, Console::Status::Info);
    Console::Print("Waiting for mgsvtpp.exe ...", Console::Status::Info);

    std::optional<ProcessInfo> proc;
    for (int i = 0; i < 60; ++i) {
        proc = FindProcessByName("mgsvtpp.exe");
        if (proc) break;
        Sleep(1000);

        if (i % 5 == 4)
            Console::Print("Still searching ... make sure the game is running", Console::Status::Warning);
    }

    if (!proc) {
        Console::Print("Timed out waiting for mgsvtpp.exe", Console::Status::Error);
        std::cout << "\n Press any key to exit...";
        std::cin.get();
        return 1;
    }

    Console::Print("Found mgsvtpp.exe  (PID: " + std::to_string(proc->pid) + ")", Console::Status::Success);
    Console::Print("Injecting ...", Console::Status::Info);

    Sleep(500);

    InjectionResult result = InjectDLL(proc->pid, dllPath);

    if (result == InjectionResult::Success) {
        Console::Print("Inner Heaven loaded successfully!", Console::Status::Success);
        Console::Print("Press INSERT in-game to open the menu", Console::Status::Info);
        Console::Print("Press END to eject the mod", Console::Status::Info);
    } else {
        Console::Print(InjectionResultToString(result), Console::Status::Error);
    }

    std::cout << "\n Press any key to exit...";
    std::cin.get();
    return (result == InjectionResult::Success) ? 0 : 1;
}
