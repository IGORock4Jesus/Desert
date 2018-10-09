#pragma once

#include "graphics_irenderer.h"
#include <D3D11.h>
#include <wrl.h>


namespace Desert::Graphics::D3D11
{
	class Renderer : public IRenderer {
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		bool initialized{ false };

	public:

		// Унаследовано через IRenderer
		virtual bool Initialize(System::Window * window) override;
		virtual void Release() override;
		virtual void Begin() override;
		virtual void End() override;

		virtual void DrawRect(float x, float y, float w, float h, const Color * color) override;
	};
}