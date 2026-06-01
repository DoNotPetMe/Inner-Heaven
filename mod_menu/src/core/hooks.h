#pragma once
#include <d3d11.h>
#include <dxgi.h>

namespace Hooks {

void Init();
void Shutdown();

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount,
                                   UINT Width, UINT Height, DXGI_FORMAT Format, UINT Flags);

} // namespace Hooks
