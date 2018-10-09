#pragma once


namespace Desert::Graphics
{
	__interface IVertexBuffer
	{
		bool Initialize(size_t size);
		void SetData(void* data, size_t size);
	};
}