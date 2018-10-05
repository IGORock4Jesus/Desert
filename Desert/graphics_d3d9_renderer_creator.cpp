#include "graphics_d3d9_renderer_creator.h"
#include "graphics_d3d9_renderer.h"

namespace Desert::Graphics::D3D9 {
	IRenderer * RendererCreator::Create()
	{
		return new Renderer;
	}

}