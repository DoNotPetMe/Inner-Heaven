#include "visuals.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <imgui.h>
#include <cmath>
#include <cstdio>

namespace Features::Visuals {

static uintptr_t s_BrightnessAddr = 0;
static uintptr_t s_VPMatrixAddr   = 0;
static uintptr_t s_EntityListAddr = 0;
static uintptr_t s_PlayerPosAddr  = 0;

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };
struct Matrix4x4 { float m[4][4]; };

static bool WorldToScreen(const Vec3& w, Vec2& s, const Matrix4x4& vp, float sw, float sh) {
    float ww = vp.m[0][3]*w.x + vp.m[1][3]*w.y + vp.m[2][3]*w.z + vp.m[3][3];
    if (ww < 0.001f) return false;
    float x = vp.m[0][0]*w.x + vp.m[1][0]*w.y + vp.m[2][0]*w.z + vp.m[3][0];
    float y = vp.m[0][1]*w.x + vp.m[1][1]*w.y + vp.m[2][1]*w.z + vp.m[3][1];
    float inv = 1.0f / ww;
    s.x = sw*0.5f + sw*0.5f * x * inv;
    s.y = sh*0.5f - sh*0.5f * y * inv;
    return true;
}

void Init() {
    uintptr_t base = Memory::GetBaseAddress();
    size_t    size = Memory::GetModuleSize();

    uintptr_t br = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? F3 0F 10 ?? ?? ?? ?? ?? 0F 2F C1 76", base, size);
    if (br) s_BrightnessAddr = br;

    uintptr_t vp = Pattern::Scan("0F 10 ?? ?? ?? ?? ?? 0F 11 ?? ?? 0F 10 ?? ?? ?? ?? ?? 0F 11 ?? ?? 0F 10 ?? ?? ?? ?? ?? 0F 11", base, size);
    if (vp) s_VPMatrixAddr = vp;

    uintptr_t el = Pattern::Scan("48 8B ?? ?? ?? ?? ?? 48 85 C0 74 ?? 48 8D ?? ?? ?? E8 ?? ?? ?? ?? 48 8B ?? 48 85 C0 74", base, size);
    if (el) s_EntityListAddr = el;

    uintptr_t pos = Pattern::Scan("F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? F3 0F 11 ?? ?? ?? ?? ?? 48 8B ?? ?? E8", base, size);
    if (pos) s_PlayerPosAddr = pos;
}

static void DrawCrosshair(ImDrawList* dl, float w, float h) {
    auto& c = Config::Get();
    float cx = w * 0.5f, cy = h * 0.5f;
    float sz = (float)c.crosshairSize;
    float gap = (float)c.crosshairGap;
    float th = (float)c.crosshairThickness;
    int alpha = (int)(c.crosshairOpacity * 255.0f);
    ImU32 col = IM_COL32(c.crosshairColorR, c.crosshairColorG, c.crosshairColorB, alpha);

    switch (c.crosshairStyle) {
    default:
    case 0: // CROSS
        dl->AddLine(ImVec2(cx-sz-gap,cy), ImVec2(cx-gap,cy), col, th);
        dl->AddLine(ImVec2(cx+gap,cy), ImVec2(cx+gap+sz,cy), col, th);
        dl->AddLine(ImVec2(cx,cy-sz-gap), ImVec2(cx,cy-gap), col, th);
        dl->AddLine(ImVec2(cx,cy+gap), ImVec2(cx,cy+gap+sz), col, th);
        dl->AddCircleFilled(ImVec2(cx,cy), th*0.8f, col, 8);
        break;
    case 1: // DOT
        dl->AddCircleFilled(ImVec2(cx,cy), th*2.0f, col, 12);
        break;
    case 2: // CIRCLE
        dl->AddCircle(ImVec2(cx,cy), sz, col, 24, th);
        dl->AddCircleFilled(ImVec2(cx,cy), th, col, 8);
        break;
    case 3: // T_SHAPE (no bottom line)
        dl->AddLine(ImVec2(cx-sz-gap,cy), ImVec2(cx-gap,cy), col, th);
        dl->AddLine(ImVec2(cx+gap,cy), ImVec2(cx+gap+sz,cy), col, th);
        dl->AddLine(ImVec2(cx,cy-sz-gap), ImVec2(cx,cy-gap), col, th);
        break;
    case 4: // CHEVRON
        dl->AddLine(ImVec2(cx-sz,cy+sz*0.5f), ImVec2(cx,cy-sz*0.3f), col, th);
        dl->AddLine(ImVec2(cx+sz,cy+sz*0.5f), ImVec2(cx,cy-sz*0.3f), col, th);
        break;
    }
}

void RenderOverlay() {
    auto& c = Config::Get();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    float sw = ImGui::GetIO().DisplaySize.x;
    float sh = ImGui::GetIO().DisplaySize.y;

    if (c.nightVision && s_BrightnessAddr)
        Memory::Write<float>(s_BrightnessAddr, c.nightVisionStr);

    if (c.crosshair)
        DrawCrosshair(dl, sw, sh);

    // FPS counter (top-right)
    if (c.showFPS) {
        char fps[32];
        snprintf(fps, sizeof(fps), "FPS: %.0f", ImGui::GetIO().Framerate);
        dl->AddText(ImVec2(sw - 100, 10), IM_COL32(0, 255, 0, 220), fps);
    }

    // Frame time
    if (c.showFrameTime) {
        char ft[32];
        snprintf(ft, sizeof(ft), "%.2f ms", 1000.0f / ImGui::GetIO().Framerate);
        float y = c.showFPS ? 28.0f : 10.0f;
        dl->AddText(ImVec2(sw - 100, y), IM_COL32(200, 200, 200, 200), ft);
    }

    // Player position display
    if (c.showPosition && s_PlayerPosAddr) {
        __try {
            float px = Memory::Read<float>(s_PlayerPosAddr);
            float py = Memory::Read<float>(s_PlayerPosAddr + 4);
            float pz = Memory::Read<float>(s_PlayerPosAddr + 8);
            char pos[64];
            snprintf(pos, sizeof(pos), "X:%.1f Y:%.1f Z:%.1f", px, py, pz);
            dl->AddText(ImVec2(10, sh - 30), IM_COL32(255, 255, 255, 200), pos);
        } __except(1) {}
    }

    if (!c.espEnabled || !s_VPMatrixAddr || !s_EntityListAddr)
        return;

    Matrix4x4 vpMatrix{};
    __try { vpMatrix = Memory::Read<Matrix4x4>(s_VPMatrixAddr); }
    __except(1) { return; }

    __try {
        uintptr_t lb = Memory::Read<uintptr_t>(s_EntityListAddr);
        if (!lb) return;
        int cnt = Memory::Read<int>(lb + 0x10);
        if (cnt <= 0 || cnt > 200) return;
        uintptr_t arr = Memory::Read<uintptr_t>(lb + 0x18);
        if (!arr) return;

        for (int i = 0; i < cnt; ++i) {
            uintptr_t ent = Memory::Read<uintptr_t>(arr + i * 8);
            if (!ent) continue;

            Vec3 pos = { Memory::Read<float>(ent+0x80), Memory::Read<float>(ent+0x84), Memory::Read<float>(ent+0x88) };
            float hp = Memory::Read<float>(ent+0x1B8);
            float mhp = Memory::Read<float>(ent+0x1BC);
            if (mhp <= 0) mhp = 1.0f;

            Vec2 sp{};
            if (!WorldToScreen(pos, sp, vpMatrix, sw, sh)) continue;
            if (sp.x < 0 || sp.x > sw || sp.y < 0 || sp.y > sh) continue;

            ImU32 col = IM_COL32(255, 50, 50, 220);

            if (c.espBoxes)
                dl->AddRect(ImVec2(sp.x-22,sp.y-44), ImVec2(sp.x+22,sp.y+9), col, 1.0f, 0, 1.5f);

            if (c.espDistance) {
                float d = sqrtf(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
                char b[32]; snprintf(b,sizeof(b),"%.0fm",d);
                dl->AddText(ImVec2(sp.x-10,sp.y+12), IM_COL32(255,255,255,200), b);
            }

            if (c.espHealthBar && mhp > 0) {
                float r = hp/mhp; if(r<0)r=0; if(r>1)r=1;
                float bw=40,bh=4,bx=sp.x-20,by=sp.y-50;
                dl->AddRectFilled(ImVec2(bx,by), ImVec2(bx+bw,by+bh), IM_COL32(40,40,40,180));
                dl->AddRectFilled(ImVec2(bx,by), ImVec2(bx+bw*r,by+bh),
                    IM_COL32((int)(255*(1-r)),(int)(255*r),0,220));
            }
        }
    } __except(1) {}
}

int GetScanFound() {
    return (s_BrightnessAddr?1:0) + (s_VPMatrixAddr?1:0)
         + (s_EntityListAddr?1:0) + (s_PlayerPosAddr?1:0);
}
int GetScanTotal() { return 4; }

int CountEnemiesInRadius(float cx, float cy, float cz, float radius) {
    if (!s_EntityListAddr) return -1;

    int found = 0;
    float r2 = radius * radius;

    __try {
        uintptr_t lb = Memory::Read<uintptr_t>(s_EntityListAddr);
        if (!lb) return -1;
        int cnt = Memory::Read<int>(lb + 0x10);
        if (cnt < 0 || cnt > 1024) return -1;
        uintptr_t arr = Memory::Read<uintptr_t>(lb + 0x18);
        if (!arr) return -1;

        for (int i = 0; i < cnt; ++i) {
            uintptr_t ent = Memory::Read<uintptr_t>(arr + i * 8);
            if (!ent) continue;

            float hp = Memory::Read<float>(ent + 0x1B8);
            if (hp <= 5.0f) continue; // dead or downed-to-zero — not an active threat

            float ex = Memory::Read<float>(ent + 0x80);
            float ey = Memory::Read<float>(ent + 0x84);
            float ez = Memory::Read<float>(ent + 0x88);
            float dx = ex - cx, dy = ey - cy, dz = ez - cz;
            if (dx*dx + dy*dy + dz*dz <= r2)
                ++found;
        }
    } __except(1) { return -1; }

    return found;
}

} // namespace Features::Visuals
