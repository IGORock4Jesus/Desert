#include "graphics_d3d11_renderer.h"
#include "log.h"
#include "system_window.h"
#include "system_file.h"


#pragma comment (lib, "d3d11.lib")


namespace Desert::Graphics::D3D11 {

	bool Renderer::LoadShaders()
	{
		shaders.insert({ "simple 2d vs", LoadShader("graphics_d3d11_simple_2d_vertex_shader.cso") });
		shaders.insert({ "simple 2d ps", LoadShader("graphics_d3d11_simple_2d_pixel_shader.cso") });
		return true;
	}

	Microsoft::WRL::ComPtr<ID3D11DeviceChild> Renderer::LoadShader(std::string filename)
	{
		System::File file{ filename };


	}

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

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		swapChain->GetBuffer(0, IID_PPV_ARGS(&texture));

		device->CreateRenderTargetView(texture.Get(), nullptr, &renderTargetView);

		// нужно еще создать буфер глубины - ибо у нас кто последним нарисовался тот и ближе
		D3D11_TEXTURE2D_DESC depthDesc{ 0 };
		depthDesc.Width = window->GetWidth();
		depthDesc.Height = window->GetHeight();
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc = { 1,0 };
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		device->CreateTexture2D(&depthDesc, nullptr, &texture);

		device->CreateDepthStencilView(texture.Get(), nullptr, &depthStencilView);

		D3D11_VIEWPORT viewport{ 0 };
		viewport.Height = depthDesc.Height;
		viewport.Width = depthDesc.Width;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);

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

		context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Renderer::End()
	{
		swapChain->Present(1, 0);
	}

	void Renderer::DrawRect(float x, float y, float w, float h, const Color * color)
	{

	}

	IVertexBuffer * Renderer::CreateVertexBuffer()
	{
		return nullptr;
	}

	IIndexBuffer * Renderer::CreateIndexBuffer()
	{
		return nullptr;
	}

}