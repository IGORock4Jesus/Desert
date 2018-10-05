#include "graphics_d3d11_renderer_creator.h"
#include "graphics_d3d11_renderer.h"

namespace Desert::Graphics::D3D11 {

	IRenderer * RendererCreator::Create()
	{
		return new Renderer;
	}

}