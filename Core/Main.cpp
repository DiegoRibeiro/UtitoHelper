#include <Windows.h>

#include "TopWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) 
{
	TopWindow window;
	window.setupAndShow(hInstance, nCmdShow);

	return 0;
}