#pragma once

#include "graphics_irenderer.h"


namespace Desert::Graphics::D3D11
{
	class Renderer : public IRenderer {

	public:

		// ������������ ����� IRenderer
		virtual bool Initialize(System::Window * window) override;
		virtual void Release() override;
		virtual void Begin() override;
		virtual void End() override;
	};
}