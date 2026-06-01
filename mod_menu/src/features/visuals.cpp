#include "visuals.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../memory/pattern.h"
#include <imgui.h>
#include <cmath>

namespace Features::Visuals {

static uintptr_t s_BrightnessAddr = 0;
static uintptr_t s_VPMatrixAddr   = 0;
static uintptr_t s_EntityListAddr = 0;

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
}

static void DrawCrosshair(ImDrawList* dl, float w, float h) {
    auto& c = Config::Get();
    float cx = w * 0.5f, cy = h * 0.5f;
    float sz = (float)c.crosshairSize, gap = sz * 0.3f;
    ImU32 col = IM_COL32(0, 255, 0, 200);
    dl->AddLine(ImVec2(cx-sz,cy), ImVec2(cx-gap,cy), col, 1.5f);
    dl->AddLine(ImVec2(cx+gap,cy), ImVec2(cx+sz,cy), col, 1.5f);
    dl->AddLine(ImVec2(cx,cy-sz), ImVec2(cx,cy-gap), col, 1.5f);
    dl->AddLine(ImVec2(cx,cy+gap), ImVec2(cx,cy+sz), col, 1.5f);
    dl->AddCircle(ImVec2(cx,cy), 2.0f, col, 12, 1.0f);
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
