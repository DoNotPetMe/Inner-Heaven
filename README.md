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

## Feature Categories (~380+ settings)

### Player
- **Combat > Weapon Cheats**: God Mode, Infinite Ammo, No Reload, Infinite Suppressors, Infinite Mags, Infinite Throwables, Infinite Battery
- **Combat > Accuracy & Recoil**: No Recoil, No Weapon Sway, Perfect Accuracy, No Bullet Drop
- **Combat > Damage & Effects**: One Hit Kill, Damage Multiplier, Explosive Bullets, Penetrating Shots, Silent Weapons
- **Combat**: Rapid Fire, Infinite Reflex, Reflex Duration
- **Movement**: Super Speed (configurable multiplier), No Fall Damage, Sprint/Crawl/Dive/Swim speed tuning
- **Movement > Physics**: Gravity Scale, Custom Player Scale
- **Stealth**: Stealth Mode (frozen detection), Noise Scale
- **Stats & Health**: Player Health Scale (0-650%), Stamina Scale, Unlimited Stamina, No Breath Limit, Grip Strength
- **Hero & Demon**: Don't Subtract Hero, Don't Add Ogre, Hero Subtracts Ogre

### Appearance
- **Character Model**: Player Type selection (Snake, DD Male/Female, Avatar, Liquid, Ocelot, Quiet), Avatar Gender
- **Outfit & Suit**: 17 suit/outfit types, DD Male/Female uniform variants
- **Camo & Paint**: Camo pattern index (0-120), Face Paint selection
- **Head & Face**: Headgear, FOVA Face/Body/Hair variation, Horn visibility, Shrapnel type
- **Prosthetic Arm**: 8 bionic arm types, 4 color variants
- **Accessories**: Scarf (6 variants), Accessory (10 types), Cigar, Blood Stains

### Camera
- **Free Camera**: WASD+Space/Ctrl movement, adjustable speed, First Person mode
- **FOV Override**: Global FOV + per-mode (Normal, Aiming, Hiding, CQC)
- **Lens & Focus**: Focal length, aperture, focus distance
- **Position Offsets**: Shoulder X/Y, camera distance, height, tilt/dutch angle
- **Zoom & Scope**: Scope and binocular zoom multipliers
- **Effects**: Camera shake intensity, auto-center disable, letterbox bars
- **Photo Mode**: Freeze frame, 6 color filters, adjustable filter strength

### Resources
- **GMP**: Set, lock, presets (5M, 50M)
- **Heroism**: Set, lock, preset (1M)
- **Demon Points**: Set, lock, zero preset
- **Resource Scale**: 8 resource type multipliers (Material, Plant, Diamond, Container, Fuel, Bio, Metal, Minor)
- **Staff & Development**: MB Coin multiplier, recruit level, auto-dismiss, processing speed, instant base dev
- Max All Plants / Materials commands

### World
- **Time Scale**: Slow motion, clock speed (1-10000x), speed camera (world/player scale, continue time)
- **Time of Day**: Override with slider + Dawn/Noon/Dusk/Midnight presets
- **Weather**: Force 7 weather types, fog density & type, dust density, rain wetness
- **Lighting**: Ambient light, sun intensity, shadow intensity, color temperature, skybox rotation
- **Wind**: Speed (0-30 m/s) and direction (0-360°)
- **Environment**: No Enemy/Animal AI, Infinite Fulton, disable hazards (mines, traps, cameras, decoys, radio), water level, destruction level
- Teleport to Waypoint

### Enemy
- **Soldier Params > Detection**: Sight/Night Sight/Hearing scale, CP communication range
- **Soldier Params > Combat**: Health, Accuracy, Reaction, Aggression, Speed scale
- **Soldier Params > Status Effects**: Stun/Sleep/Holdup duration scale
- **Behavior**: Disable radio calls, body search, investigation, alert propagation, force interrogation, comms jamming
- **Phases**: Force alert phase (5 types), Keep Phase lock
- **Reinforcements**: Force super reinforce, heli reinforce, count/level ranges
- **Custom Prep > Weapons**: 6 weapon types prevalence
- **Custom Prep > Armor**: 5 defensive gear types
- **Custom Prep > Equipment**: 5 utility types (decoy, mine, camera, flashlight, radio)
- **Custom Prep > Explosives**: 4 explosive types (grenade, smoke, stun, C4)
- **Custom Prep > Quality**: Weapon and armor development levels
- **Appearance**: Soldier face/body variation, custom soldier type
- **Vehicle Patrols**: Count and type (Truck/APC/Tank/Random)
- **Special Forces**: Wild Card, LRRP, force snipers, force heavy armor
- Revenge mode (4 modes)

### Skulls / Parasite
- **Spawning**: Enable/disable, free roam spawns, type (Mist/Camo/Armor/Metallic), count (1-12), respawn timer
- **Stats**: Health/Speed/Damage/Aggression scale
- **Behavior**: Teleport, disappear, force phase (Melee/Ranged/Snipe), armor break
- Parasite mist density

### Mission
- **Preparation**: Skip prep, Start on Foot, Force Night/Day
- **Restrictions > Core Rules**: Game Over on Discovery, Disable Game Over/Retry/Out of Bounds, No Checkpoints
- **Restrictions > Loadout**: Subsistence, Clear Items, Disable Heli Attack, Hand Ability/Item level
- **Restrictions > Difficulty Mods**: No Reflex/Marking/Supply Drop/Buddy Support/Air Strikes/Spotter/Indicators/SloMo
- **Restrictions > Timed Mission**: Enable time limit, configurable minutes
- **HUD & Markers**: Disable head/world/X-ray markers
- **Support Menus**: Disable support/buddy/attack/fulton menus
- **Score & Ranking**: Score multiplier, mission rank override (S through E)

### Helicopter
- **Defense**: Invincible, Disable Pull Out, Stealth, Auto-Evade, Armor scale
- **Landing & Approach**: Search Light, LZ Wait Height, Door Open Time, Disable LZs, Approach Angle
- **Armament**: Weapon selection (Minigun/Rocket/Gatling/None), Speed scale
- **Customization**: Color/skin (12 variants), custom music, radio station

### Buddy
- **Quiet**: 6 weapon variants, behavior mode (Scout/Attack/Cover), bond level, humming, radio (32 tracks), invisibility
- **D-Dog**: 5 equipment loadouts, behavior (Guard/Scout/Attack), bond level, barking, auto-mark, tricks (Shake/Roll/Sit/All)
- **D-Horse**: 4 equipment variants, behavior (Follow/Stay/Wander), bond level, defecation, speed scale
- **D-Walker**: 5 weapon loadouts, behavior (Follow/Patrol/Guard), ammo count, auto-attack, invisibility

### Mother Base
- **Characters**: Toggle Ocelot, Puppy, Code Talker, Birds, Buddies, Animals (5 species)
- **Soldiers**: Equip range, additional spawns, prioritize female, lethal actions, morale, NPC route mode
- **Patrols**: Support/Attack heli count, Walker Gears
- **Assets**: Posters, collection repop, unlock goal doors
- **Ocean**: Wave height/speed, RGB color, foam, transparency, reflection
- **Security**: Security level (5 tiers), alarms, UAV drones
- **Facilities**: Lighting mode, shower timer, platform selection

### Fulton
- **Mode**: Auto-fulton, recover critical, disable fulton, silent fulton
- **Level & Quality**: Fulton/wormhole level, success rate override
- **Targets**: Fulton everything, vehicles, containers, gun emplacements
- **Penalties & Variation**: Variation range, dying/sleep penalties, extraction speed

### Side Ops
- **Selection**: Force quest number, selection mode, repop mode, max active quests
- **Rules**: Enable during missions, retry, enemy scaling/level
- **Time Limits**: Optional time limits with configurable duration

### Cutscenes
- **Character**: Use selected soldier in cutscenes
- **Playback**: Allow action, skip all, playback speed (0.1-4x), free camera
- **Overrides**: Weather and time of day during cutscenes
- **Display**: Hide HUD, subtitle mode, letterbox bars

### Events
- **Free Roam Events**: HUNTED, CRASHLAND, LOST COMS, AMBUSH, SANDSTORM, BLACKOUT, INVASION, SUPPLY LOSS (0-100% chance each)
- **Skull Encounters**: Random Skull unit spawns in free roam with configurable chance
- **MB War Games**: 8 faction profiles
- **Wildlife**: Animal encounter rate, meteor showers, custom event frequency

### Vehicle (Inner Heaven Exclusive)
- **Durability**: God mode, no damage, health scale, auto-repair
- **Performance**: Speed multiplier, handling preset (Arcade/Realistic/Drift), nitro boost
- **Weapons**: Infinite vehicle ammo
- **Spawning**: Spawn vehicle by type (Truck/Jeep/APC/Tank/Walker), color selection
- **Misc**: Invisibility, horn, radio station

### Sound / Music (Inner Heaven Exclusive)
- **Volume**: Per-channel volume (BGM, SFX, Voice, Ambient, Radio)
- **Mute**: Quick mute toggles per channel
- **Cassette Player**: Track selection (60 tracks), loop, autoplay
- **Combat Audio**: Alert BGM override, disable alert/reflex sounds, heli music override

### Progression (Inner Heaven Exclusive)
- **Unlock All**: Weapons, Items, Outfits, Buddy Gear, Blueprints, Tapes, Missions, Side Ops
- **Team Levels**: Combat, R&D, Support, Intel, Medical, Base Dev, Security (0-99)
- **Max Out**: Max platforms, max teams, force recruit rank (S++ through A)

### Wave Survival — Custom Gamemode (Inner Heaven Exclusive)
A self-contained stealth wave-survival mode inspired by *Splinter Cell: Blacklist*'s wave maps. Picked from the **Wave Survival** menu, every arena is always replayable.

- **Start Mission**: 6 selectable arenas (Afghan Outpost, Africa Village, Mother Base Deck, Ruins Compound, Airport Facility, Current Location). Each builds the arena around your current position.
- **Enemy counter HUD** (top-left): live count of remaining hostiles, current wave number, stealth/detected status, and running score.
- **Wave loop**: thin out the enemies (stealthily or loud) — when the counter hits **zero**, a larger reinforcement wave spawns in. Survive the configured number of waves (or go endless).
- **Neutral until spotted**: reinforcements spawn **passive** and only turn hostile the moment you're actually detected — so each wave can be opened with a clean stealth approach before it escalates, keeping the difficulty curve fair.
- **Rules**: starting enemy count, per-wave increment, max simultaneous enemies, total waves (0 = endless), difficulty preset (Recruit/Normal/Hard/Insane), spawn-ring radius, reinforcement delay.
- **Balancing**: ghost bonus for clearing a wave undetected, optional score penalty for going loud.
- **Scoring**: per-wave bonus scaled by wave number and difficulty, with a stealth multiplier.

The enemy counter is driven by **real game state** — it reuses the live entity-list reader (the same one ESP uses) to count living enemies inside the arena, so the count reflects actual takedowns rather than a script's bookkeeping. The two game-specific touch points — spawning soldiers and reading the "player detected" flag — are delegated to an injected Lua helper (`IHWave.*`), guarded with `pcall`, so they're a single tuning seam per game build.

> **Note on the side-ops menu:** the gamemode is launched from Inner Heaven's own **Wave Survival** menu, which acts as the always-replayable mission select. Wiring these into the in-game iDroid *Side Ops* list (so they appear alongside base-game side jobs) requires hooking Fox Engine's quest registration and is scaffolded as the next integration step — the gamemode logic itself is complete and runs today from the mod menu.

### Visuals (Inner Heaven Exclusive)
- **ESP > Display**: Bounding boxes, distance, health bars, names, skeleton, weapons, alert state
- **ESP > Range & Lines**: Max range (10-2000m), snap lines with origin selection
- **ESP > Colors**: Per-channel RGB for enemies and allies
- **ESP > Extra Targets**: Vehicles, items, objectives
- **Night Vision**: Brightness amplification, 4 color modes (Green/White/Amber/Blue), contrast, film grain
- **Thermal Vision**: Heat imaging, 4 palettes (Ironbow/Rainbow/White Hot/Black Hot)
- **Crosshair**: 5 styles (Cross/Dot/Circle/T-Shape/Chevron), size/gap/thickness/opacity, dynamic expansion, RGB color
- Color grading filter (6 presets)

### Quick Presets (Inner Heaven Exclusive)
- **Stealth Preset**: Silent assassin configuration
- **Rambo Preset**: Full combat god mode
- **Hardcore Preset**: Maximum difficulty challenge
- **Exploration Preset**: Free roam with no enemies
- **Photo Preset**: Photography mode setup
- **Max Resources**: Instant max GMP/Heroism/zero Demon
- **Reset All**: Return all settings to default

### Misc
- **Position Slots**: Save/Load up to 3 world positions
- Lua Console (open from menu)
- Show Help toggle

### Debug
- **Logging**: Debug mode, messages, button printing, file logging, 5 verbosity levels
- **Display**: Position, FPS counter, frame time
- **Rendering**: Wireframe, collision, nav mesh, patrol routes, CP zone boundaries
- **Startup**: Skip logos, start offline
- Memory dump

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
- [MinHook](https://github.com/TsudaKageyu/minhook) — x86/x64 API hooking

Both are included as git submodules.

## Acknowledgments

- [TinManTex/IHHook](https://github.com/TinManTex/IHHook) — reference for DX11 hooking and Lua API patterns
- [Infinite Heaven](https://www.nexusmods.com/metalgearsolidvtpp/mods/45) — inspiration for menu design and feature scope
- MGSV modding community CE tables — verified AOB patterns for health, ammo, and more

## Disclaimer

This tool is for **singleplayer use only**. Do not use it in MGSV's FOB (online) mode. Using mods online may result in a ban. The developers of Inner Heaven are not responsible for any consequences of misuse.

## License

MIT
