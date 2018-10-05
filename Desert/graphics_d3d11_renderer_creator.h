#pragma once

#include "graphics_irenderer_creator.h"


namespace Desert::Graphics::D3D11
{
	class RendererCreator : public IRendererCreator {
		// Унаследовано через IRendererCreator
		virtual IRenderer * Create() override;
	};
}