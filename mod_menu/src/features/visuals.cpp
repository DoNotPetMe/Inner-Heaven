#include "visuals.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <imgui.h>
#include <cmath>

namespace Features::Visuals {

static State s_State;

// FOV: IHHook uses UpdateFOVLerp, focal length at thisptr + 0x2FC
// Default focal lengths: TPP=21.0, Shoulder=22.0
static uintptr_t s_FOVAddr        = 0;
static uintptr_t s_BrightnessAddr = 0;
static uintptr_t s_VPMatrixAddr   = 0;
static uintptr_t s_EntityListAddr = 0;
static float     s_OrigFOV        = 0.0f;

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };
struct Matrix4x4 { float m[4][4]; };

State& GetState() { return s_State; }

static bool WorldToScreen(const Vec3& world, Vec2& screen, const Matrix4x4& vp,
                           float screenW, float screenH) {
    float w = vp.m[0][3] * world.x + vp.m[1][3] * world.y + vp.m[2][3] * world.z + vp.m[3][3];
    if (w < 0.001f) return false;

    float x = vp.m[0][0] * world.x + vp.m[1][0] * world.y + vp.m[2][0] * world.z + vp.m[3][0];
    float y = vp.m[0][1] * world.x + vp.m[1][1] * world.y + vp.m[2][1] * world.z + vp.m[3][1];

    float invW = 1.0f / w;
    screen.x = (screenW * 0.5f) + (screenW * 0.5f) * x * invW;
    screen.y = (screenH * 0.5f) - (screenH * 0.5f) * y * invW;

    return true;
}

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    // UpdateFOVLerp — verified IHHook pattern (Fox Engine camera system)
    uintptr_t fovScan = Pattern::Scan(
        "4C 8B DC 49 89 5B ?? 55 56 57 41 54 41 57 49 8D AB", base, size
    );
    if (fovScan) {
        // The focal length float is at thisptr + 0x2FC inside the function
        s_FOVAddr = fovScan;
    }

    // Scene brightness / post-process exposure
    uintptr_t brightScan = Pattern::Scan(
        "F3 0F 11 ?? ?? ?? ?? ?? F3 0F 10 ?? ?? ?? ?? ?? 0F 2F C1 76", base, size
    );
    if (brightScan) {
        s_BrightnessAddr = brightScan;
    }

    // View-Projection matrix — used for world-to-screen in ESP
    uintptr_t vpScan = Pattern::Scan(
        "0F 10 ?? ?? ?? ?? ?? 0F 11 ?? ?? 0F 10 ?? ?? ?? ?? ?? 0F 11 ?? ?? 0F 10 ?? ?? ?? ?? ?? 0F 11", base, size
    );
    if (vpScan) {
        s_VPMatrixAddr = vpScan;
    }

    // Entity list / NPC manager
    uintptr_t entityScan = Pattern::Scan(
        "48 8B ?? ?? ?? ?? ?? 48 85 C0 74 ?? 48 8D ?? ?? ?? E8 ?? ?? ?? ?? 48 8B ?? 48 85 C0 74", base, size
    );
    if (entityScan) {
        s_EntityListAddr = entityScan;
    }
}

static void DrawCrosshair(ImDrawList* drawList, float screenW, float screenH) {
    float cx = screenW * 0.5f;
    float cy = screenH * 0.5f;
    float sz = static_cast<float>(s_State.crosshairSize);
    ImU32 col = IM_COL32(0, 255, 0, 200);
    float gap = sz * 0.3f;

    // Tactical crosshair with center gap
    drawList->AddLine(ImVec2(cx - sz, cy), ImVec2(cx - gap, cy), col, 1.5f);
    drawList->AddLine(ImVec2(cx + gap, cy), ImVec2(cx + sz, cy), col, 1.5f);
    drawList->AddLine(ImVec2(cx, cy - sz), ImVec2(cx, cy - gap), col, 1.5f);
    drawList->AddLine(ImVec2(cx, cy + gap), ImVec2(cx, cy + sz), col, 1.5f);
    drawList->AddCircle(ImVec2(cx, cy), 2.0f, col, 12, 1.0f);
}

void RenderOverlay() {
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float screenW = displaySize.x;
    float screenH = displaySize.y;

    // Custom FOV — write the focal length value
    // Fox Engine: focalLength ~21.0 = ~75deg FOV, lower = wider
    static bool prevFOV = false;
    if (s_State.customFOV && s_FOVAddr) {
        // Convert desired FOV degrees to approximate Fox Engine focal length
        // Relationship: focalLength ≈ 18.0 / tan(fov_rad / 2) * tan(75_rad / 2)
        float fovRad = s_State.fovValue * 0.01745329f;
        float defaultFovRad = 75.0f * 0.01745329f;
        float focalLength = 21.0f * (tanf(defaultFovRad * 0.5f) / tanf(fovRad * 0.5f));
        // Note: actual write requires the camera struct pointer, resolved at runtime
        (void)focalLength;
    }
    prevFOV = s_State.customFOV;

    // Night vision
    if (s_State.nightVision && s_BrightnessAddr) {
        Memory::Write<float>(s_BrightnessAddr, s_State.nightVisionStr);
    }

    // Crosshair
    if (s_State.crosshair) {
        DrawCrosshair(drawList, screenW, screenH);
    }

    // ESP
    if (!s_State.espEnabled || !s_VPMatrixAddr || !s_EntityListAddr)
        return;

    Matrix4x4 vpMatrix{};
    __try {
        vpMatrix = Memory::Read<Matrix4x4>(s_VPMatrixAddr);
    } __except(1) {
        return;
    }

    __try {
        uintptr_t listBase = Memory::Read<uintptr_t>(s_EntityListAddr);
        if (!listBase) return;

        int count = Memory::Read<int>(listBase + 0x10);
        if (count <= 0 || count > 200) return;

        uintptr_t arrayPtr = Memory::Read<uintptr_t>(listBase + 0x18);
        if (!arrayPtr) return;

        for (int i = 0; i < count; ++i) {
            uintptr_t entity = Memory::Read<uintptr_t>(arrayPtr + i * 8);
            if (!entity) continue;

            // Fox Engine TransformData: position at offset 0x80
            Vec3 pos{};
            pos.x = Memory::Read<float>(entity + 0x80);
            pos.y = Memory::Read<float>(entity + 0x84);
            pos.z = Memory::Read<float>(entity + 0x88);

            float health    = Memory::Read<float>(entity + 0x1B8);
            float maxHealth = Memory::Read<float>(entity + 0x1BC);
            if (maxHealth <= 0.0f) maxHealth = 1.0f;

            Vec2 screenPos{};
            if (!WorldToScreen(pos, screenPos, vpMatrix, screenW, screenH))
                continue;

            if (screenPos.x < 0 || screenPos.x > screenW ||
                screenPos.y < 0 || screenPos.y > screenH)
                continue;

            ImU32 boxColor = IM_COL32(255, 50, 50, 220);

            if (s_State.espBoxes) {
                float boxW = 22.0f;
                float boxH = 44.0f;
                drawList->AddRect(
                    ImVec2(screenPos.x - boxW, screenPos.y - boxH),
                    ImVec2(screenPos.x + boxW, screenPos.y + boxH * 0.2f),
                    boxColor, 1.0f, 0, 1.5f
                );
            }

            if (s_State.espDistance) {
                float dist = std::sqrtf(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
                char distBuf[32];
                snprintf(distBuf, sizeof(distBuf), "%.0fm", dist);
                drawList->AddText(
                    ImVec2(screenPos.x - 10, screenPos.y + 12),
                    IM_COL32(255, 255, 255, 200), distBuf
                );
            }

            if (s_State.espHealthBar && maxHealth > 0.0f) {
                float ratio = health / maxHealth;
                if (ratio < 0.0f) ratio = 0.0f;
                if (ratio > 1.0f) ratio = 1.0f;

                float barW = 40.0f, barH = 4.0f;
                float barX = screenPos.x - barW * 0.5f;
                float barY = screenPos.y - 50.0f;

                drawList->AddRectFilled(
                    ImVec2(barX, barY), ImVec2(barX + barW, barY + barH),
                    IM_COL32(40, 40, 40, 180)
                );
                drawList->AddRectFilled(
                    ImVec2(barX, barY), ImVec2(barX + barW * ratio, barY + barH),
                    IM_COL32(
                        static_cast<int>(255 * (1.0f - ratio)),
                        static_cast<int>(255 * ratio), 0, 220
                    )
                );
            }
        }
    } __except(1) {
        // Silently handle access violations from stale entity pointers
    }
}

} // namespace Features::Visuals
