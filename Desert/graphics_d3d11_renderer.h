#pragma once

#include "graphics_irenderer.h"
#include <D3D11.h>
#include <wrl.h>
#include <map>


namespace Desert::Graphics::D3D11
{
	class Renderer : public IRenderer {
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

		std::map<std::string, Microsoft::WRL::ComPtr<ID3D11DeviceChild>> shaders;


		bool initialized{ false };

		bool LoadShaders();
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> LoadShader(std::string filename);

	public:

		// Унаследовано через IRenderer
		virtual bool Initialize(System::Window * window) override;
		virtual void Release() override;
		virtual void Begin() override;
		virtual void End() override;

		virtual void DrawRect(float x, float y, float w, float h, const Color * color) override;

		// Унаследовано через IRenderer
		virtual IVertexBuffer * CreateVertexBuffer() override;
		virtual IIndexBuffer * CreateIndexBuffer() override;
	};
}