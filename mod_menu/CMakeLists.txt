add_library(InnerHeavenMenu SHARED
    src/dllmain.cpp
    src/core/hooks.cpp
    src/core/input.cpp
    src/core/renderer.cpp
    src/memory/memory.cpp
    src/memory/pattern.cpp
    src/menu/menu.cpp
    src/menu/menu_system.cpp
    src/menu/menu_defs.cpp
    src/menu/styles.cpp
    src/features/player.cpp
    src/features/resources.cpp
    src/features/world.cpp
    src/features/visuals.cpp
    src/features/misc.cpp
    src/features/lua_console.cpp
    src/features/game_lua.cpp
    src/features/wavemode.cpp
    src/features/buddies.cpp
)

target_include_directories(InnerHeavenMenu PRIVATE src)
target_link_libraries(InnerHeavenMenu PRIVATE imgui minhook d3d11 dxgi d3dcompiler xinput)
