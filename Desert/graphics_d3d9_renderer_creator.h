#pragma once

#include "graphics_irenderer_creator.h"

namespace Desert::Graphics::D3D9
{
	class RendererCreator : public IRendererCreator {
	public:
		virtual IRenderer* Create() override;
	};
}
