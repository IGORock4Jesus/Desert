#pragma once

#include "graphics_irenderer_creator.h"


namespace Desert::Graphics::D3D11
{
	class RendererCreator : public IRendererCreator {
		// ������������ ����� IRendererCreator
		virtual IRenderer * Create() override;
	};
}