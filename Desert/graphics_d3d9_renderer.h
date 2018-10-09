#pragma once

#include "graphics_irenderer.h"
#include <d3d9.h>
#include "event.h"


namespace Desert::Graphics::D3D9
{
	class Renderer : public IRenderer {
		LPDIRECT3D9 direct;
		LPDIRECT3DDEVICE9 device;
		
	public:
		Event<IRenderer*> Drawing;

		// Унаследовано через IRenderer
		virtual bool Initialize(System::Window * window) override;
		virtual void Release() override;
		virtual void Begin() override;
		virtual void End() override;

		virtual void DrawRect(float x, float y, float w, float h, const Color* color) override;

		virtual IVertexBuffer * CreateVertexBuffer() override;
		virtual IIndexBuffer * CreateIndexBuffer() override;
	};
}