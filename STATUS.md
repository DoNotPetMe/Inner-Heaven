# Project Status — read this first

**Current state: the menu/overlay renders, but in-game cheats do not work on
current `mgsvtpp.exe` builds.** This is an honest accounting of why, and what
to do about it.

## What works

- DLL injection and the DirectX 11 overlay (Dear ImGui) render correctly.
- Menu navigation, config, and all the **visual-only** overlay pieces that
  don't depend on reading/writing game memory: crosshair, FPS / frame-time,
  on-screen text. (ESP needs a valid entity-list address to actually draw
  boxes — see below.)

## What does not work, and why

Every game-affecting feature resolves its target address by scanning
`mgsvtpp.exe` for a **byte-signature (AOB)**. There are two layers:

### Lua bridge — REWORKED, ported from IHHook

The original bridge used hand-guessed signatures and read **1/5** scans (dead).
It has since been rewritten using the verified, maintained signatures from
[IHHook](https://github.com/TinManTex/IHHook):

- The game has **no `luaL_loadstring`** — the old bridge scanned for a function
  that doesn't exist. It now compiles via **`luaL_loadbuffer`**.
- Uses IHHook's version-independent AOB patterns for `lua_pcall`,
  `luaL_loadbuffer`, `lua_settop`, `lua_tolstring`.
- Captures `lua_State` live from the `lua_pcall` detour (IHHook's approach),
  not a guessed global.
- Runs all queued Lua on the **game thread** (drained inside the pcall hook),
  not the render thread.

This *should* connect on current builds — verify in **Debug → Pattern Scan
Report** (expect `LUA BRIDGE: CONNECTED`). When it's green, the ~60 Lua
features (god mode via `Player.ChangeLifeMaxValue`, GMP, weather, time,
appearance, etc.) become live.

### Memory patches — still unverified guesses

The direct byte-patch features (the `player.cpp` set) are separate from the Lua
bridge and **still use guessed patterns**. The Scan Report shows ~5/11 "FOUND",
but several patterns are short/generic (e.g. Rapid Fire keys off `0F 2F`, a
`comiss` occurring in thousands of places), so "FOUND" usually means the
*wrong* instruction and NOP-ing it does nothing. These need real CE patterns
per build — or just use the Lua equivalents now that the bridge works (e.g. Lua
god mode instead of the HP-write patch).

### Wave Survival — engine-constrained

Confirmed from IH's source: **MGSV has no runtime "spawn a soldier at XYZ"
primitive** — not in the game, not in IH, not exposed by IHHook. Enemy presence
is authored per-region data (routes / command posts / ScriptBlocks). The only
runtime spawn path is **reinforcements** (`TppReinforceBlock`), which require
the current area/mission to *already* have a reinforce block
(`mvars.reinforce_hasReinforceBlock`) and a real command post — and they arrive
by helicopter. So Wave Survival can only spawn where the game already supports
reinforcements (outposts / bases / reinforcement-enabled missions); it cannot
conjure enemies in empty terrain. The C++ wave logic, HUD, scoring and
live enemy-counting all work; the spawn step is the engine-limited seam.

The code *logic* is sound — patch apply/restore, the game-thread Lua queue,
the menu — it is simply pointed at **wrong addresses**. No field-name or
threading change can fix that; only correct signatures for your specific game
build can.

## The realistic path: Infinite Heaven

[Infinite Heaven](https://www.nexusmods.com/metalgearsolidvtpp/mods/45) and its
native hook [IHHook](https://github.com/TinManTex/IHHook) (both by TinManTex)
already solve MGSV Lua injection correctly and are **maintained for current
game versions**. IHHook keeps a real, connected Lua state and runs Lua
reliably on the game thread — exactly the bridge that reads 1/5 here.

Almost everything this menu *attempts* is a real, working feature in IH's
in-game menu: health / god mode, GMP / heroism / demon points, weather and
time of day, enemy sight / hearing / behavior, buddy control and equipment,
fulton tweaks, player appearance, **soldier spawning / free-roam patrols**
(the spawn-based mode — IH does it by editing the data layer and reloading the
region, which is the only way it can be done), and much more.

**If the goal is working singleplayer cheats today:** install Infinite Heaven
(follow the official install steps on its GitHub / Nexus page, since they track
game updates) and use its menu.

## If you want THIS project to work

Two technical models, in order of effort:

1. **Scope this overlay to what IH doesn't do.** Keep the DX11 ImGui overlay
   for visual-only features (crosshair, FPS, position, ESP once wired) and let
   IH own all game-state cheating. No fragile signatures required.
2. **Build on IHHook's already-located Lua state** instead of re-scanning for
   `lua_pcall` yourself. IHHook has no clean public C-API for a third-party
   DLL, so this means coupling to IH internals (it is open-source, so it is
   doable), and you still depend on IH being loaded.
3. **Supply correct signatures for your build.** If you can provide your exact
   `mgsvtpp.exe` version and/or the addresses from a Cheat Engine session, the
   existing architecture can be pointed at them and will work. The Pattern
   Scan Report is built for exactly this cross-checking.

## Honesty note on the README

The README describes the *intended* feature set. Claims there that AOB
patterns are "verified against community CE tables" or "sourced from IHHook"
are **aspirational, not actual** — the patterns are unverified guesses. Treat
the README as a design document, and this file as the ground truth.
