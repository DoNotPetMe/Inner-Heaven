add_library(InnerHeavenMenu SHARED
    src/dllmain.cpp
    src/core/hooks.cpp
    src/core/input.cpp
    src/core/renderer.cpp
    src/memory/memory.cpp
    src/memory/pattern.cpp
    src/menu/menu.cpp
    src/menu/styles.cpp
    src/menu/tabs/tab_player.cpp
    src/menu/tabs/tab_resources.cpp
    src/menu/tabs/tab_world.cpp
    src/menu/tabs/tab_visuals.cpp
    src/menu/tabs/tab_misc.cpp
    src/features/player.cpp
    src/features/resources.cpp
    src/features/world.cpp
    src/features/visuals.cpp
    src/features/misc.cpp
    src/features/lua_console.cpp
)

target_include_directories(InnerHeavenMenu PRIVATE src)
target_link_libraries(InnerHeavenMenu PRIVATE imgui minhook d3d11 dxgi d3dcompiler)
