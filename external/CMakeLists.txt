# ── MinHook ──────────────────────────────────────────────────────────────────
# MinHook ships its own CMakeLists.txt; just forward into it.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/minhook/CMakeLists.txt")
    add_subdirectory(minhook)
else()
    message(WARNING "MinHook submodule not found – run: git submodule update --init")
endif()

# ── Dear ImGui ──────────────────────────────────────────────────────────────
# ImGui has no CMakeLists.txt, so we build it as a static library ourselves.
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

if(EXISTS "${IMGUI_DIR}/imgui.cpp")
    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    )

    target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )

    target_link_libraries(imgui PUBLIC d3d11 dxgi d3dcompiler)
else()
    message(WARNING "ImGui submodule not found – run: git submodule update --init")
endif()
