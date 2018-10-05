#pragma once


namespace Desert::Graphics
{
	__interface IGraphics;

	__interface IDrawable
	{
		void Draw(IGraphics* graphics);
	};
}