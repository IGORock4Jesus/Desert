#pragma once

namespace Desert::System {
	class Window;
}

namespace Desert::Graphics
{
	struct Color;

	__interface IRenderer
	{
		bool Initialize(System::Window* window);
		void Release();

		void Begin();
		void End();

		void DrawRect(float x, float y, float w, float h, const Color* color);
	};
}