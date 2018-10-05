#include "graphics_manager.h"
#include "graphics_irenderer_creator.h"

namespace Desert::Graphics {
	void Manager::OnPass()
	{
		if (!_renderer) return;

		_renderer->Begin();



		_renderer->End();
	}
	Manager::Manager()
		:Task("Graphics")
	{
	}
	void Manager::Initialize(IRendererCreator * creator, System::Window* window)
	{
		if (_renderer) return;
		Stop();
		_renderer = creator->Create();
		_renderer->Initialize(window);
	}

	void Manager::Release()
	{
		if (!_renderer) return;
		Stop();
		_renderer->Release();
		delete _renderer;
		_renderer = nullptr;
	}


}