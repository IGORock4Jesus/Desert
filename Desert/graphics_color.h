#pragma once


namespace Desert::Graphics
{
	struct Color
	{
		float r, g, b, a;
	};

	namespace Colors {
		constexpr Color White{ 1.0f, 1.0f, 1.0f, 1.0f };
	}
}