#include <plugin.h>

HINSTANCE hinstance;


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ulReasonForCall, LPVOID plReserved) {
	hinstance = hModule;
	if (!ControlsInit) {
		ControlsInit = true;
		InitCustomControls(hinstance);
		InitCommonControls();
	}

	return TRUE;
}

__declspec(dllexport) int LibNumberClasses() {
	return 1;
}








