#include "hooks.h"
#include "input.h"
#include "renderer.h"
#include "../menu/menu.h"
#include "../features/player.h"
#include "../features/resources.h"
#include "../features/world.h"
#include "../features/visuals.h"
#include "../features/misc.h"
#include <MinHook.h>

namespace Hooks {

using PresentFn       = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffersFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

static PresentFn       oPresent       = nullptr;
static ResizeBuffersFn oResizeBuffers = nullptr;
static bool            s_Initialized  = false;

static void* s_PresentAddr       = nullptr;
static void* s_ResizeBuffersAddr = nullptr;

static void ResolveAddresses() {
    WNDCLASSEXA wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = DefWindowProcA;
    wc.hInstance      = GetModuleHandleA(nullptr);
    wc.lpszClassName  = "IH_DX11_DUMMY";
    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(0, wc.lpszClassName, "", WS_OVERLAPPEDWINDOW,
                                0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount       = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow      = hwnd;
    sd.SampleDesc.Count  = 1;
    sd.Windowed          = TRUE;
    sd.SwapEffect        = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain*      pSwapChain = nullptr;
    ID3D11Device*        pDevice    = nullptr;
    ID3D11DeviceContext* pContext   = nullptr;

    D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &pSwapChain, &pDevice, nullptr, &pContext
    );

    void** vTable = *reinterpret_cast<void***>(pSwapChain);
    s_PresentAddr       = vTable[8];
    s_ResizeBuffersAddr = vTable[13];

    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();
    DestroyWindow(hwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
}

void Init() {
    ResolveAddresses();

    MH_Initialize();
    MH_CreateHook(s_PresentAddr, &hkPresent, reinterpret_cast<void**>(&oPresent));
    MH_CreateHook(s_ResizeBuffersAddr, &hkResizeBuffers, reinterpret_cast<void**>(&oResizeBuffers));
    MH_EnableHook(MH_ALL_HOOKS);
}

void Shutdown() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    Input::Shutdown();
    Renderer::Shutdown();
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!s_Initialized) {
        ID3D11Device* pDevice = nullptr;
        pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pDevice));

        ID3D11DeviceContext* pContext = nullptr;
        pDevice->GetImmediateContext(&pContext);

        DXGI_SWAP_CHAIN_DESC desc;
        pSwapChain->GetDesc(&desc);

        Renderer::Init(pDevice, pContext, pSwapChain, desc.OutputWindow);
        Input::Init(desc.OutputWindow);

        Features::Player::Init();
        Features::Resources::Init();
        Features::World::Init();
        Features::Visuals::Init();
        Features::Misc::Init();

        pDevice->Release();
        pContext->Release();
        s_Initialized = true;
    }

    Features::Player::Tick();
    Features::Resources::Tick();
    Features::World::Tick();
    Features::Misc::Tick();

    Renderer::BeginFrame();

    if (Input::IsMenuOpen())
        Menu::Render();

    Features::Visuals::RenderOverlay();

    Renderer::EndFrame();

    return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount,
                                   UINT Width, UINT Height, DXGI_FORMAT Format, UINT Flags) {
    Renderer::InvalidateRenderTarget();
    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, Format, Flags);
    Renderer::CreateRenderTarget(pSwapChain);
    return hr;
}

} // namespace Hooks
