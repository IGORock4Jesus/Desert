#pragma once

#include "graphics_irenderer.h"

namespace Desert::Graphics
{
	__interface IRendererCreator
	{
		IRenderer* Create();
	};
}
