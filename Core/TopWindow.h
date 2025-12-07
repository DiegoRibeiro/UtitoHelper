#pragma once

#include <Windows.h>
#include <mmsystem.h>
#include <vector>

#include "resource.h"

#pragma comment(lib, "winmm.lib")

class TopWindow
{
private:
	HWND _hwnd;
	HBITMAP _hCurrentBitmap;
	HBITMAP _hExori;
	HBITMAP _hExoriGran;
	BOOL isBloodRageActive;
	static TopWindow* s_instance;

public:
	void setupAndShow(HINSTANCE hInstance, int nCmdShow);
private:
	BOOL handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
};

