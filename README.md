# Inner Heaven

A singleplayer mod menu and DLL injector for **Metal Gear Solid V: The Phantom Pain**.

> *"The world calls for wetwork, and we answer. No greater good. No just cause."*

## Features

### Player
- God Mode — take no damage
- Infinite Ammo — never run out of ammunition
- No Reload — skip magazine changes entirely
- Infinite Suppressors — suppressors never wear out
- No Recoil — perfectly stable aim
- One Hit Kill — enemies die in a single hit
- Stealth Mode — detection meter is frozen at zero
- Infinite Reflex Mode — reflex time never expires
- Super Speed — configurable movement speed multiplier

### Resources
- GMP Editor — set or lock your GMP to any value
- Heroism Editor — set or lock heroism
- Demon Points Editor — control your horn growth
- Max All Plants / Materials — fill all resource slots
- Quick presets: Max GMP (5M), Max Heroism (1M), Zero Demon Points

### World
- Slow Motion — configurable time scale (0.05–0.9x)
- Time of Day — slider with Dawn/Noon/Dusk/Night presets
- Weather Control — Clear, Cloudy, Rainy, Foggy, Sandstorm
- No Enemy AI — freeze all enemy behavior
- Infinite Fulton — unlimited extractions
- Teleport to Waypoint — instantly move to your map marker

### Visuals
- ESP — enemy bounding boxes, distance readouts, health bars
- Custom FOV — 40–140 degree field of view
- Night Vision — brightness amplification
- Crosshair Overlay — customizable size

### Misc
- Save / Load Position — bookmark and return to any location
- Free Camera — detached camera with WASD + Space/Ctrl movement
- Custom Player Scale — resize Snake (0.1x – 5.0x)
- Rapid Fire — remove fire rate limiter
- No Fall Damage — survive any drop

### Lua Console
- **Built-in Fox Engine Lua 5.1 console** — execute any game Lua command live
- Command history with Up/Down arrow navigation
- Syntax error and runtime error reporting
- Quick-execute buttons: Max HP, Reflex ON, Supply Drop, Clear Weather
- Run any `TppMission`, `Player`, `TppWeather`, `GkEventTimerManager` call directly

## Controls

| Key | Action |
|-----|--------|
| INSERT | Toggle mod menu |
| END | Eject the mod cleanly |

## Building

### Prerequisites
- Visual Studio 2019+ or MSVC build tools
- CMake 3.20+
- Git (for submodules)

### Steps

```bash
git clone --recursive https://github.com/donotpetme/inner-heaven.git
cd inner-heaven
cmake -B build -A x64
cmake --build build --config Release
```

Output:
- `build/bin/Release/InnerHeavenInjector.exe`
- `build/bin/Release/InnerHeavenMenu.dll`

### Usage

1. Place `InnerHeavenInjector.exe` and `InnerHeavenMenu.dll` in the same folder.
2. Launch MGSV: The Phantom Pain.
3. Run `InnerHeavenInjector.exe` **as Administrator**.
4. Press **INSERT** in-game to open the menu.

## Technical Details

- **Injection method**: `CreateRemoteThread` + `LoadLibraryA` — straightforward and reliable since MGSV has no anti-cheat in singleplayer.
- **Rendering hook**: DirectX 11 `IDXGISwapChain::Present` hook via MinHook trampolines.
- **Overlay**: Dear ImGui with a custom MGS-inspired dark military green theme.
- **Memory manipulation**: AOB (Array of Bytes) pattern scanning for version-independent address resolution. Byte patching for toggleable features, per-frame value writes for continuous effects. Health and ammo patterns verified against community Cheat Engine tables.
- **Lua bridge**: Captures the game's embedded Lua 5.1 state at runtime via AOB patterns from IHHook, then exposes the full Fox Engine Lua API through an in-menu console.

## Dependencies

- [Dear ImGui](https://github.com/ocornut/imgui) — immediate-mode GUI
- [MinHook](https://github.com/TsudaKageworker/minhook) — x86/x64 API hooking

Both are included as git submodules.

## Disclaimer

This tool is for **singleplayer use only**. Do not use it in MGSV's FOB (online) mode. Using mods online may result in a ban. The developers of Inner Heaven are not responsible for any consequences of misuse.

## License

MIT
