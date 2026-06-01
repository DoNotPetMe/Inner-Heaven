#pragma once
#include <d3d11.h>
#include <dxgi.h>

namespace Renderer {

void Init(ID3D11Device* device, ID3D11DeviceContext* context,
          IDXGISwapChain* swapChain, HWND hwnd);
void Shutdown();
void BeginFrame();
void EndFrame();
void InvalidateRenderTarget();
void CreateRenderTarget(IDXGISwapChain* swapChain);

} // namespace Renderer
