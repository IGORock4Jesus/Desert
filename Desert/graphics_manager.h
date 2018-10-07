#pragma once

#include "system_window.h"
#include "system_task.h"
#include "event.h"


namespace Desert::Graphics
{
	__interface IRenderer;
	__interface IRendererCreator;

	class Manager : public System::Task
	{
		IRenderer* _renderer{ nullptr };


		virtual void OnPass() override;

	public:
		Event<Manager, IRenderer*> Drawing;

		Manager();

		void Initialize(IRendererCreator* creator, System::Window* window);
		void Release();

		IRenderer* GetRenderer() const { return _renderer; }
	};
}