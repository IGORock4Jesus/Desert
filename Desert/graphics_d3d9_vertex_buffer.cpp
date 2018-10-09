#include "graphics_d3d9_vertex_buffer.h"

namespace Desert::Graphics::D3D9
{

	VertexBuffer::VertexBuffer(LPDIRECT3DDEVICE9 device)
		:device{ device }, buffer{ nullptr }
	{
	}

	bool VertexBuffer::Initialize(size_t size)
	{
		if (buffer)
			return false;

		device->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, )
	}

	void VertexBuffer::SetData(void * data, size_t size)
	{
		if (!buffer) return;
		buffer->Release();
		buffer = nullptr;
	}

}