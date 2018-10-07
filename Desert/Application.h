#pragma once
#include "system_window.h"
#include "graphics_manager.h"


namespace Desert {

	class Application
	{
		HINSTANCE _hinstance;
		System::Window* _window;
		Graphics::Manager* _graphics;
		bool enableRendering{ false };

		void OnKeyDown(BYTE byte);
		void OnDrawing(Graphics::IRenderer* renderer);

	public:
		Application(HINSTANCE hinstance);
		virtual ~Application();

		void Run();
	};

}