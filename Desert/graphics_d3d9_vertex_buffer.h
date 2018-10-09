#pragma once

#include "graphics_vertex_buffer.h"
#include <d3d9.h>


namespace Desert::Graphics::D3D9
{
	class VertexBuffer : public IVertexBuffer
	{
		LPDIRECT3DVERTEXBUFFER9 buffer;
		LPDIRECT3DDEVICE9 device;

	public:
		VertexBuffer(LPDIRECT3DDEVICE9 device);

		// Унаследовано через IVertexBuffer
		virtual bool Initialize(size_t size) override;
		virtual void SetData(void * data, size_t size) override;
	};
}