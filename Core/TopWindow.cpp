#include "TopWindow.h"

TopWindow* TopWindow::s_instance = nullptr;

void TopWindow::setupAndShow(HINSTANCE hInstance, int nCmdShow)
{
	s_instance = this;

	WNDCLASS windowClass = {0};
	windowClass.lpfnWndProc = TopWindow::windowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = L"UtitoHelper";
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&windowClass);

	_hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, L"UtitoHelper", L"Utito Helper", WS_POPUP, 0, 0, 38, 38, nullptr, nullptr, hInstance, this);

	if (!_hwnd) {
		return;
	}

	SetLayeredWindowAttributes(_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	ShowWindow(_hwnd, nCmdShow);
	UpdateWindow(_hwnd);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x06;
	rid.dwFlags = RIDEV_INPUTSINK; // Or 0 if you want only when focused
	rid.hwndTarget = _hwnd;
	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
		MessageBox(_hwnd, L"Failed to register raw input device!", L"Error", MB_ICONERROR);
	}

	MSG msg;
	BOOL ret;
	while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) {
			MessageBox(_hwnd, L"Error loop message", L"Error", MB_ICONERROR);
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//MessageBox(nullptr, L"Shutting down", L"Shutdown", MB_OK);
}

BOOL TopWindow::handleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		isBloodRageActive = FALSE;

		_hExori = (HBITMAP)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_EXORI), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		_hExoriGran = (HBITMAP)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_EXORI_GRAN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		if (!_hExori || !_hExoriGran) {
			MessageBox(hwnd, L"Failed to load bitmap resources!", L"Error", MB_ICONERROR);
		}

		_hCurrentBitmap = _hExori;

		return TRUE;
	//case WM_HOTKEY:
	//	if (wParam == 1 && !isBloodRageActive)
	//	{
	//		isBloodRageActive = TRUE;
	//		SetTimer(hwnd, 1, 10000, nullptr);

	//		_hCurrentBitmap = _hExoriGran;
	//		InvalidateRect(hwnd, nullptr, TRUE);
	//	}

	//	return TRUE;

	case WM_INPUT:
	{
		UINT dwSize = 0;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
		if (dwSize > 0) {
			std::vector<BYTE> lpb(dwSize);
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.data());
				if (raw->header.dwType == RIM_TYPEKEYBOARD) {
					RAWKEYBOARD& rawKB = raw->data.keyboard;
					USHORT vKey = rawKB.VKey;
					USHORT message = rawKB.Message;

					if ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) && vKey == 'I' && !isBloodRageActive) {
						isBloodRageActive = TRUE;
						_hCurrentBitmap = _hExoriGran;
						InvalidateRect(_hwnd, nullptr, TRUE);

						SetTimer(_hwnd, 1, 10000, nullptr);
					}
				}
			}
		}
		return FALSE;
	}

	case WM_TIMER:
		if (wParam == 1) {
			PlaySound(MAKEINTRESOURCE(IDR_NOTIFY), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
			KillTimer(hwnd, 1);
			isBloodRageActive = FALSE;
			_hCurrentBitmap = _hExori;
			InvalidateRect(hwnd, nullptr, TRUE);
		}

		return TRUE;

	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct;
		HDC hdc = BeginPaint(hwnd, &paintStruct);

		FillRect(hdc, &paintStruct.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));

		if (_hCurrentBitmap) {
			HDC memDC = CreateCompatibleDC(hdc);
			HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, _hCurrentBitmap);
			BITMAP bmp;
			GetObject(_hCurrentBitmap, sizeof(bmp), &bmp);
			BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, memDC, 0, 0, SRCCOPY);
			SelectObject(memDC, oldBmp);
			DeleteDC(memDC);
		}

		EndPaint(hwnd, &paintStruct);
		return TRUE;
	}
	case WM_LBUTTONDOWN:
		ReleaseCapture();
		SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return TRUE;

	case WM_RBUTTONDOWN:
		PostQuitMessage(0);
		return TRUE;

	case WM_DESTROY:
		KillTimer(hwnd, 1);

		if (_hExori) {
			DeleteObject(_hExori);
		}
		if (_hExoriGran) {
			DeleteObject(_hExoriGran);
		}

		PostQuitMessage(0);
		return TRUE;

	default:
		return FALSE;
	}
}

LRESULT TopWindow::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		TopWindow* pThis = reinterpret_cast<TopWindow*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		TopWindow* pThis = reinterpret_cast<TopWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		if (pThis) {
			if (pThis->handleMessage(hwnd, uMsg, wParam, lParam)) {
				return 0;
			}
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
