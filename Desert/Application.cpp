#include "Application.h"
#include "graphics_d3d9_renderer_creator.h"
#include "graphics_d3d11_renderer_creator.h"
#include "graphics_color.h"



namespace Desert {
	void Application::OnKeyDown(BYTE byte)
	{
		if (byte == 0x39) {
			_graphics->Release();
			Graphics::D3D9::RendererCreator creator;
			_graphics->Initialize(&creator, _window);
		}
		if (byte == 0x31) {
			_graphics->Release();
			Graphics::D3D11::RendererCreator creator;
			_graphics->Initialize(&creator, _window);
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

	void Application::OnDrawing(Graphics::IRenderer * renderer)
	{
		renderer->DrawRect(100, 100, 200, 50, &Graphics::Colors::White);
	}

	Application::Application(HINSTANCE hinstance)
		:_hinstance{ hinstance }
	{
		_window = new System::Window(hinstance);
		_window->KeyDown += std::pair{ this, &Application::OnKeyDown };

		_graphics = new Graphics::Manager();
		_graphics->Drawing += std::pair(this, &Application::OnDrawing);
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