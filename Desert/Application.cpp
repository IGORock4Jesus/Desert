#include "Application.h"
#include "graphics_d3d9_renderer_creator.h"
#include "graphics_d3d11_renderer_creator.h"


namespace Desert {
	void Application::OnKeyDown(BYTE byte)
	{
		if (byte == 0x39) {
			_graphics->Release();
			_graphics->Initialize(&Graphics::D3D9::RendererCreator(), _window);
		}
		if (byte == 0x31) {
			_graphics->Release();
			_graphics->Initialize(&Graphics::D3D11::RendererCreator(), _window);
		}

		if (byte == VK_SPACE) {
			enableRendering = !enableRendering;
			if (enableRendering) {
				_graphics->Start();
			}
			else {
				_graphics->Stop();
			}
		}
	}
	Application::Application(HINSTANCE hinstance)
		:_hinstance{ hinstance }
	{
		_window = new System::Window(hinstance);
		_window->KeyDown += std::pair{ this, &Application::OnKeyDown };

		_graphics = new Graphics::Manager();

	}


	Application::~Application()
	{
		delete _graphics;
		delete _window;
	}

	void Application::Run()
	{
		_window->EnterMessageLoop();
	}

}