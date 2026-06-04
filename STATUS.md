# Project Status — read this first

**Current state: the menu/overlay renders, the Lua bridge connects, and
verified Lua features work in-game.** Fabricated field names have been removed.

## What works

### Overlay & UI
- DLL injection and DirectX 11 overlay (Dear ImGui) render correctly.
- Menu navigation, config persistence, all visual overlays (crosshair, FPS,
  frame-time, on-screen text).
- Lua Console with live bridge status, command history, quick-action buttons.

### Lua bridge (IHHook-style) — WORKING
The bridge uses IHHook's verified AOB patterns for `lua_pcall`,
`luaL_loadbuffer`, `lua_settop`, `lua_tolstring`. It captures `lua_State`
live from the pcall detour and runs all Lua on the game thread. Check
**Debug → Pattern Scan Report** — expect `LUA BRIDGE: CONNECTED`.

### Verified Lua features (these actually do something)
Every Lua call in `game_lua.cpp` Tick() has been cross-referenced against
[mgsv-deminified-lua](https://github.com/TinManTex/mgsv-deminified-lua) and
[InfiniteHeaven](https://github.com/TinManTex/InfiniteHeaven).

| Feature | Lua API | Notes |
|---------|---------|-------|
| God mode | `Player.ChangeLifeMaxValue(50000)` | Reasserted every 3s |
| Player health scale | `Player.ChangeLifeMaxValue(N)` | On change |
| Infinite ammo / no reload | `SendCommand(TppPlayer2, ResetAmmo)` | Every 500ms |
| Infinite suppressor | `SendCommand(TppPlayer2, ResetSuppressor)` | Every 500ms |
| Super speed | `SendCommand(TppPlayer2, SetStandMoveSpeedLimit)` | On change |
| Unlimited stamina | `SendCommand(TppPlayer2, ResetStamina)` | Every 500ms |
| No fall damage | `Player.ChangeLifeMaxValue(0)` (HP reset) | Backup for memory patch |
| No reflex | `vars.playerDisableActionFlag` | PlayerDisableAction.REFLEXMODE |
| Game over on discovery | `TppMission.RegistDiscoveryGameOver()` | On toggle |
| Appearance (6 fields) | `vars.playerType`, `playerPartsType`, `playerCamoType`, `playerFaceEquipId`, `handEquip`, `playerFaceId` | On change each |
| Slow motion | `HighSpeedCamera.RequestEvent{}` / `RequestToCancel()` | On change |
| Time of day | `TppClock.SetTime()` / `TppClock.Start()` | On change |
| Force night / day | `TppClock.SetTime("02:00"/"12:00")` + `TppClock.Stop()` | On toggle |
| Weather control | `TppWeather.ForceRequestWeather(id, duration)` | On change |
| Clock time scale | `TppClock.SetTimeScale(N)` | On change |
| Helicopter invincible | `TppHelicopter.SetInvincible(bool)` | On toggle |
| Helicopter searchlight | `TppHelicopter.SetSearchLight(bool)` | On change |
| Helicopter forced stay | `TppHelicopter.SetForcedStay(bool)` | On toggle |
| Helicopter stealth | `TppHelicopter.SetStealth(bool)` | On toggle |
| Set GMP | `TppMotherBaseManagement.SetGmp{gmp=N}` | On change / lock |
| Set Heroism | `TppMotherBaseManagement.SetHeroicPoint{N}` | On change / lock |
| Set Demon points | `TppMotherBaseManagement.SetOgrePoint{N}` | On change / lock |
| Hero/Ogre lock | Periodic reassertion of SetHeroicPoint/SetOgrePoint | Every 2.5s |
| Enemy prep (revenge) | `TppRevenge.SetRevengePoint(type, pts)` | Real REVENGE_TYPE enum |
| Skip cutscenes | `DemoDaemon.SkipAll()` | Fixed from wrong `TppDemo.Skip` |
| Fulton 100% success | `mvars.ply_allways_100percent_fulton` | Verified field |
| Teleport (save/load) | `vars.playerPosX/Y/Z` + `TppPlayer.Warp{}` | 3 slots |

### Memory patch features (AOB-based, some working)
These live in `player.cpp`, `world.cpp`, `resources.cpp` and don't use Lua:
- God mode (HP write NOP) — pattern-dependent
- Infinite ammo / no reload — pattern-dependent
- No recoil / no sway — pattern-dependent
- AI disable (world.cpp) — working via memory patch
- Infinite fulton count (world.cpp) — working via memory patch
- GMP/heroism/demon direct write (resources.cpp) — pattern-dependent

### Buddy command wheel
`buddies.cpp` provides a Lua-based buddy command system using the game's
`GameObject.SendCommand` with verified `TppBuddyService` command types.

## What does NOT work (removed fabricated Lua calls)

These features have UI toggles but **no working backend**. The Lua field
names they used were fabricated — they don't exist in the game's Lua
environment. Writing to them was a silent no-op (C++ userdata `__newindex`
drops unknown keys).

| Feature | Fabricated field | Reality |
|---------|-----------------|---------|
| Noise scale | `vars.ply_noiseLevelRate` | No such field |
| Silent weapons | `vars.ply_isNoWeaponNoise` | No such field |
| Enemy sight/hearing | `gvars.soldierSightDistRate` etc. | IH uses `TppSoldier2.ReloadSoldier2ParameterTables` with modified Lua tables — too complex for DLL-only |
| Disable radio | `gvars.ene_disableRadioCall` | No such field |
| No alert propagation | `gvars.ene_noAlertPropagation` | No such field |
| Disable game over | `mvars.mis_isDisableGameOver` | IH hooks `TppMission.ReserveGameOver` |
| No marking | `gvars.mis_noMarking` | No such field |
| Subsistence mode | `mvars.mis_isSubsistence` | Per-mission, not a runtime flag |
| Enemy phase control | `TppMission.SetPhase()` | Doesn't exist; phases are per-CP via SendCommand |
| Buddy equip IDs | `vars.quietWeaponId` etc. | No such fields |
| Demo soldier | `vars.demoIsUseSoldier` | IH Ivar, not a game field |
| MB characters | `gvars.mb_isEnableOcelot` etc. | IH Ivars, not game fields |
| Vehicle god/ammo | `vars.veh_isInvincible` etc. | No such fields |
| Skulls in free roam | `gvars.skl_isEnableSkulls` | IH Ivar, not a game field |
| Enemy AI (Lua) | `gvars.ene_isDisableAI` | No such field; memory patch in world.cpp works |
| Unlock all weapons | `TppMotherBaseManagement.UnlockAllWeaponBlueprint()` | Function doesn't exist |
| Unlock all items | `TppMotherBaseManagement.UnlockAllItemBlueprint()` | Function doesn't exist |
| Unlock all missions | `TppMission.UnlockAllMission()` | Function doesn't exist |
| Unlock all side ops | `TppQuest.UnlockAllQuest()` | Function doesn't exist |
| Force start quest | `TppQuest.ForceStartQuest(N)` | Function doesn't exist |
| Play cassette | `TppUiCommand.PlayCassette(N)` | Function doesn't exist |
| Night vision | `GrTools.SetBrightness(N)` | Function doesn't exist |
| Avatar gender | `vars.avatarGender` | Not in deminified source |
| Body variation | `vars.playerBodyId` | Not in deminified source |

## Paths forward for broken features

1. **Memory patches (AOB):** Some features (noise, vehicle, AI) could work
   via direct memory writes if correct byte signatures are found with Cheat
   Engine for your specific build.

2. **Load Infinite Heaven alongside this mod:** Many of the "impossible"
   features (sight/hearing, game over disable, skulls, MB characters) are
   implemented in IH through its own `Ivars` system and deep Lua hooks.
   With IH loaded, this mod could call IH's APIs instead.

3. **Accept the limitation:** Some features simply can't be done from a DLL
   without the engine-level hooks that IH provides. The UI toggles are kept
   as placeholders for when a solution becomes available.
