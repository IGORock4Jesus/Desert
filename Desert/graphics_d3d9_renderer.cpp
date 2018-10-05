#include "graphics_d3d9_renderer.h"
#include "log.h"
#include "system_window.h"

#include <d3d9.h>

#pragma comment (lib, "d3d9.lib")

namespace Desert::Graphics::D3D9
{
	bool Renderer::Initialize(System::Window * window)
	{
		Debug::Info("D3D9 Renderer - Initialize");

		if (!(direct = Direct3DCreate9(D3D_SDK_VERSION)))
			return false;

		D3DPRESENT_PARAMETERS pp{ 0 };
		pp.AutoDepthStencilFormat = D3DFMT_D24S8;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;

		pp.BackBufferHeight = window->GetHeight();
		pp.BackBufferWidth = window->GetWidth();
		pp.EnableAutoDepthStencil = TRUE;
		pp.hDeviceWindow = window->GetHandle();
		pp.SwapEffect = D3DSWAPEFFECT::D3DSWAPEFFECT_DISCARD;
		pp.Windowed = TRUE;

		HRESULT result = S_OK;
		if (FAILED(result = direct->CreateDevice(0, D3DDEVTYPE_HAL, window->GetHandle(), D3DCREATE_MULTITHREADED|D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device)))
			return false;

		return true;
	}

	void Desert::Graphics::D3D9::Renderer::Release()
	{
		Debug::Info("D3D9 Renderer - Release");

		if (device) {
			device->Release();
			device = nullptr;
		}
		if (direct) {
			direct->Release();
			direct = nullptr;
		}
	}

	void Desert::Graphics::D3D9::Renderer::Begin()
	{
		device->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0xff303030, 1.0f, 0);
		device->BeginScene();
	}

	void Desert::Graphics::D3D9::Renderer::End()
	{
		device->EndScene();
		device->Present(nullptr, nullptr, nullptr, nullptr);
	}
}