#include "graphics_d3d11_renderer.h"
#include "log.h"
#include "system_window.h"

#pragma comment (lib, "d3d11.lib")


namespace Desert::Graphics::D3D11 {

	bool Renderer::Initialize(System::Window * window)
	{
		if (initialized) return true;

		Debug::Info("D3D11 Renderer - Initialize");


		D3D_FEATURE_LEVEL featureLevels[]{
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
		};
		D3D_FEATURE_LEVEL featureLevel;
		DXGI_SWAP_CHAIN_DESC swapChainDesc{ 0 };
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.Height = window->GetHeight();
		swapChainDesc.BufferDesc.RefreshRate = { 1, 60 };
		swapChainDesc.BufferDesc.Width = window->GetWidth();
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = window->GetHandle();
		swapChainDesc.SampleDesc = { 1,0 };
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Windowed = true;

		D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &context);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);

		initialized = true;

		return true;
	}

	void Renderer::Release()
	{
		Debug::Info("D3D11 Renderer - Release");

		renderTargetView = nullptr;
		swapChain = nullptr;
		context = nullptr;
		device = nullptr;

		initialized = false;
	}

	void Renderer::Begin()
	{
		if (!initialized)return;

		float backColor[]{ 1.0f, 0.4f, 0.4f, 1.0f };
		context->ClearRenderTargetView(renderTargetView.Get(), backColor);

	}

	void Renderer::End()
	{
		swapChain->Present(1, 0);
	}

	void Renderer::DrawRect(float x, float y, float w, float h, const Color * color)
	{

	}

}