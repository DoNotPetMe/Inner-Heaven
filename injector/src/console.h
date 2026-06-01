#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

namespace Console {

enum class Status { Info, Success, Error, Warning };

inline void SetColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

inline void ResetColor() {
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

inline void PrintBanner() {
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << R"(
  ___                        _   _
 |_ _|_ __  _ __   ___ _ __| | | | ___  __ ___   _____ _ __
  | || '_ \| '_ \ / _ \ '__| |_| |/ _ \/ _` \ \ / / _ \ '_ \
  | || | | | | | |  __/ |  |  _  |  __/ (_| |\ V /  __/ | | |
 |___|_| |_|_| |_|\___|_|  |_| |_|\___|\__,_| \_/ \___|_| |_|
)" << '\n';

    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "              MGSV: The Phantom Pain - Trainer\n";
    SetColor(FOREGROUND_GREEN);
    std::cout << "              ─────────────────────────────────\n\n";
    ResetColor();
}

inline void Print(const std::string& msg, Status status) {
    switch (status) {
    case Status::Info:
        SetColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << " [*] ";
        break;
    case Status::Success:
        SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << " [+] ";
        break;
    case Status::Error:
        SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << " [-] ";
        break;
    case Status::Warning:
        SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << " [!] ";
        break;
    }
    ResetColor();
    std::cout << msg << '\n';
}

} // namespace Console
