#include "Application.h"


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, LPSTR, int) {
	Desert::Application app{ hinstance };
	app.Run();
	return 0;
}