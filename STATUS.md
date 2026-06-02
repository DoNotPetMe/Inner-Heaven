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
`mgsvtpp.exe` for a hard-coded **byte-signature (AOB)**. Those signatures in
this repo were written by hand and **were never validated against a real game
binary**. The in-menu **Debug → Pattern Scan Report** confirms it on a live
game:

- **Lua bridge: ~1/5 scans resolve.** `lua_pcall` / `luaL_loadstring` /
  `lua_State` are not found, so the bridge never connects. Roughly 60 features
  (god mode, resources, weather, time, appearance, enemy, mission, buddy,
  Mother Base, progression, the Wave Survival hooks, etc.) run through this
  bridge and therefore do nothing.
- **Memory patches: ~5/11 "FOUND", but unreliable.** Several patterns are
  short and generic (e.g. Rapid Fire keys off `0F 2F`, a `comiss` that occurs
  in thousands of places). "FOUND" only means *some* bytes matched
  *somewhere* — almost always the wrong instruction, so NOP-ing it has no
  visible effect.

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
