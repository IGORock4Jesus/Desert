#include "graphics_d3d11_renderer.h"
#include "log.h"


namespace Desert::Graphics::D3D11 {

	bool Renderer::Initialize(System::Window * window)
	{
		Debug::Info("D3D11 Renderer - Initialize");
		return false;
	}

	void Renderer::Release()
	{
		Debug::Info("D3D11 Renderer - Release");
	}

	void Renderer::Begin()
	{
	}

	void Renderer::End()
	{
	}

	void Renderer::DrawRect(float x, float y, float w, float h, const Color * color)
	{
	}

}