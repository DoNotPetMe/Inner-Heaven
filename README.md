# Inner Heaven

A singleplayer mod menu and DLL injector for **Metal Gear Solid V: The Phantom Pain**.

Inspired by [Infinite Heaven](https://www.nexusmods.com/metalgearsolidvtpp/mods/45) by TinManTex — rebuilt from scratch with a unique feature set and our own implementation.

> *"The world calls for wetwork, and we answer. No greater good. No just cause."*

## Menu System

Inner Heaven uses a **hierarchical list-based menu** modeled after Infinite Heaven's navigation:

- **Arrow keys** — Navigate up/down, change values left/right
- **Enter** — Enter submenus, toggle options, execute commands
- **Escape / Backspace** — Go back to parent menu
- **INSERT or F3** — Toggle menu open/close
- **END** — Eject the mod cleanly

The menu shows a breadcrumb path at the top, a scrollable item list, navigation hints, and context-sensitive help text at the bottom.

## Feature Categories (~160+ settings)

### Player
- **Combat**: God Mode, Infinite Ammo, No Reload, Infinite Suppressors, No Recoil, One Hit Kill, Infinite Reflex, Rapid Fire
- **Movement**: Super Speed (configurable multiplier), No Fall Damage, Custom Player Scale
- **Stealth**: Stealth Mode (frozen detection)
- **Stats**: Player Health Scale (0-650%), Hero/Demon point controls

### Appearance
- Player Type selection (Snake, DD Male/Female, Avatar, Liquid, Ocelot, Quiet)
- 17 suit/outfit types (Sneaking Suit, Battle Dress, Parasite Suit, MGS1, Raiden, Ninja, Tuxedo, etc.)
- Camo pattern index, headgear, bionic arm type

### Camera
- Free Camera with WASD+Space/Ctrl movement and adjustable speed
- FOV override per mode (Normal, Aiming, Hiding, CQC)
- Focal length, aperture, and focus distance controls

### Resources
- GMP, Heroism, Demon Points — set, lock, or use presets (5M GMP, 1M Heroism, Zero Demon)
- Max All Plants / Materials
- Resource collection scale per type (Material, Plant, Diamond, Container: 10-1000%)

### World
- **Time Scale**: Slow motion, clock speed (1-10000x), speed camera controls
- **Time of Day**: Override with slider + Dawn/Noon/Dusk/Midnight presets
- **Weather**: Force Sunny/Cloudy/Rainy/Sandstorm/Foggy/Pouring, fog density & type
- No Enemy AI, Infinite Fulton, Teleport to Waypoint

### Enemy
- **Soldier Params**: Health/Sight/Night Sight/Hearing scale (0-400%), Item Drop Chance
- **Phases**: Force alert phase (Sneak/Caution/Evasion/Alert), Keep Phase lock
- **Reinforcements**: Force super reinforce, heli reinforce, count/level ranges
- **Custom Prep**: Per-equipment prevalence (Sniper, Missile, MG, Shotgun, SMG, Armor, Shield, Helmet, NVG, Gas Mask, Decoy, Mine, Camera)
- Revenge mode, Wild Card soldiers, LRRP foot patrols

### Mission
- Skip preparation, Start on Foot, Subsistence mode
- Gameplay restrictions: Game Over on Discovery, Disable Game Over/Retry/Out of Bounds
- HUD: Disable head/world/X-ray markers
- Support menu controls: Disable support/buddy/attack/fulton menus

### Helicopter
- Invincible Heli, Search Light override, Disable Pull Out
- LZ Wait Height, Door Open Time, Disable Landing Zones

### Buddy
- Quiet weapon selection (6 variants with suppressed options)
- D-Dog equipment (Naked, Sneaking Stun/Knife, Battle Dress, Fulton)
- D-Horse and D-Walker loadout selection

### Mother Base
- **Characters**: Toggle Ocelot, Puppy, Code Talker, Birds, Buddies
- **Soldiers**: Equip range, additional spawns, prioritize female, lethal actions, morale
- **Patrols**: Support/Attack heli count, Walker Gears
- **Assets**: Posters, collection repop, unlock goal doors

### Fulton
- Auto-fulton mode, recover critical, fulton/wormhole level
- Variation range, dying/sleep penalties

### Side Ops
- Force quest number, selection mode, repop mode
- Enable quests during story missions, shooting practice retry

### Cutscenes
- Use selected soldier in all cutscenes
- Force action during demos, weather/time overrides

### Events
- Free roam events: HUNTED, CRASHLAND, LOST COMS (0-100% chance each)
- MB War Games with 8 faction profiles

### Visuals (Inner Heaven Exclusive)
- **ESP**: Bounding boxes, distance, health bars for enemies
- **Night Vision**: Brightness amplification with adjustable intensity
- **Crosshair**: Tactical overlay with gap design, adjustable size

### Misc
- Save/Load Position
- Lua Console (open from menu)
- Show Help toggle, Debug controls

### Lua Console (Inner Heaven Exclusive)
- **Live Fox Engine Lua 5.1 console** — execute any game command interactively
- Command history with Up/Down arrows
- Quick-execute buttons for common operations
- Full error reporting with syntax and runtime distinction

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
4. Press **INSERT** or **F3** in-game to open the menu.

## Technical Details

- **Injection**: `CreateRemoteThread` + `LoadLibraryA` — no anti-cheat in MGSV singleplayer
- **Rendering**: DirectX 11 `IDXGISwapChain::Present` hook (vtable index 8) via MinHook
- **Overlay**: Dear ImGui with IH-inspired dark theme and hierarchical ListBox navigation
- **Memory**: AOB pattern scanning for version-independent addresses. Health (`F3 0F 11 0E`) and ammo (`66 44 89 1C 48`) patterns verified against community CE tables
- **Lua Bridge**: Captures the game's Lua 5.1 state at runtime, exposes full Fox Engine Lua API. Lua patterns sourced from IHHook (TinManTex)
- **Feature Application**: Byte patching for toggles, per-frame value writes for continuous effects, Lua execution for game-system features

## Dependencies

- [Dear ImGui](https://github.com/ocornut/imgui) — immediate-mode GUI
- [MinHook](https://github.com/TsudaKageworker/minhook) — x86/x64 API hooking

Both are included as git submodules.

## Acknowledgments

- [TinManTex/IHHook](https://github.com/TinManTex/IHHook) — reference for DX11 hooking and Lua API patterns
- [Infinite Heaven](https://www.nexusmods.com/metalgearsolidvtpp/mods/45) — inspiration for menu design and feature scope
- MGSV modding community CE tables — verified AOB patterns for health, ammo, and more

## Disclaimer

This tool is for **singleplayer use only**. Do not use it in MGSV's FOB (online) mode. Using mods online may result in a ban. The developers of Inner Heaven are not responsible for any consequences of misuse.

## License

MIT
