#include "renderer.h"
#include "../menu/styles.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

namespace Renderer {

static ID3D11Device*           s_Device     = nullptr;
static ID3D11DeviceContext*    s_Context    = nullptr;
static ID3D11RenderTargetView* s_RTV       = nullptr;
static bool                    s_Ready      = false;

void CreateRenderTarget(IDXGISwapChain* swapChain) {
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    if (backBuffer) {
        s_Device->CreateRenderTargetView(backBuffer, nullptr, &s_RTV);
        backBuffer->Release();
    }
}

void InvalidateRenderTarget() {
    if (s_RTV) {
        s_RTV->Release();
        s_RTV = nullptr;
    }
}

void Init(ID3D11Device* device, ID3D11DeviceContext* context,
          IDXGISwapChain* swapChain, HWND hwnd) {
    s_Device  = device;
    s_Context = context;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    Styles::Apply();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    CreateRenderTarget(swapChain);
    s_Ready = true;
}

void Shutdown() {
    if (!s_Ready) return;
    InvalidateRenderTarget();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    s_Ready = false;
}

void BeginFrame() {
    if (!s_Ready) return;
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void EndFrame() {
    if (!s_Ready) return;
    ImGui::Render();
    s_Context->OMSetRenderTargets(1, &s_RTV, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Renderer
