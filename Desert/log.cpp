#include "log.h"
#include <Windows.h>

void Desert::Debug::Info(std::string text)
{
	OutputDebugString((text + "\n").c_str());
}
