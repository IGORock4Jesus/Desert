#pragma once

namespace Desert::System {
	class Window;
}

namespace Desert::Graphics
{
	struct Color;
	__interface IVertexBuffer;
	__interface IIndexBuffer;


	__interface IRenderer
	{
		bool Initialize(System::Window* window);
		void Release();

		void Begin();
		void End();

		void DrawRect(float x, float y, float w, float h, const Color* color);

		IVertexBuffer* CreateVertexBuffer();
		IIndexBuffer* CreateIndexBuffer();
	};
}