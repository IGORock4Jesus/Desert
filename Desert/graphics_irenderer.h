#pragma once

namespace Desert::System {
	class Window;
}

namespace Desert::Graphics
{

	__interface IRenderer
	{
		bool Initialize(System::Window* window);
		void Release();

		void Begin();
		void End();
	};
}