#include "graphics_d3d9_renderer.h"
#include "log.h"
#include "system_window.h"
#include "graphics_color.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#ifdef _DEBUG
#pragma comment (lib, "d3dx9d.lib")
#else
#pragma comment (lib, "d3dx9.lib")
#endif // _DEBUG


namespace Desert::Graphics::D3D9
{
	struct Vertex2D
	{
		D3DXVECTOR4 pos;
		D3DCOLOR color;
	};
	constexpr DWORD Vertex2D_Format = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	constexpr DWORD Vertex2D_Size = sizeof(Vertex2D);


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
		if (FAILED(result = direct->CreateDevice(0, D3DDEVTYPE_HAL, window->GetHandle(), D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device)))
			return false;

		return true;
	}

	void Renderer::Release()
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

	void Renderer::Begin()
	{
		device->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, 0xff303030, 1.0f, 0);
		device->BeginScene();
	}

	void Renderer::End()
	{
		device->EndScene();
		device->Present(nullptr, nullptr, nullptr, nullptr);
	}

	D3DCOLOR toD3DCOLOR(const Color* color) {
		return ((UINT)(color->a * 255) << 24) +
			((UINT)(color->r * 255) << 16) +
			((UINT)(color->g * 255) << 8) +
			((UINT)(color->b * 255));
	}

	void Renderer::DrawRect(float x, float y, float w, float h, const Color* color)
	{
		device->SetFVF(Vertex2D_Format);
		Vertex2D vs[]{
			{{x,y,0,1}, toD3DCOLOR(color)},
			{{x + w,y,0,1}, toD3DCOLOR(color)},
			{{x + w,y + h,0,1}, toD3DCOLOR(color)},
			{{x,y + h,0,1}, toD3DCOLOR(color)},
		};
		device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vs, Vertex2D_Size);
	}

}