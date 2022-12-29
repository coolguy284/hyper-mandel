#include <Windows.h>
#include <CommCtrl.h>

#include "../resources/icon.h"

#include "consts.h"
#include "globals.h"
#include "wnd_proc.h"

// declare the extern globals

UIElem_T UIElems;
HINSTANCE mainHInstance;
HWND mainHWnd;
WIDHEIGHT windowSize = { 0 };
WIDHEIGHT renderSize = { 0 };
mandel::calc::Coords mandelCoords;

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	// assign main window instance
	
	mainHInstance = hInstance;
	
	// create window class
	
	WNDCLASSEX wcex = { 0 };
	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
#ifdef _NOICON
	wcex.hIcon = (HICON)LoadImage(wcex.hInstance, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
#else
	wcex.hIcon = (HICON)LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED); // loaded from a file but not explicitly (loaded from exe itself, specified in resource file), don't know if LR_SHARED is a problem
#endif
	wcex.hCursor = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) /* white */;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
#ifdef _NOICON
	wcex.hIconSm = (HICON)LoadImage(wcex.hInstance, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
#else
	wcex.hIconSm = (HICON)LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED); // loaded from a file but not explicitly (loaded from exe itself, specified in resource file), don't know if LR_SHARED is a problem
#endif
	
	// register window class
	
	if (!RegisterClassEx(&wcex)) {
		// register failure
		
		MessageBox(NULL,
			L"Call to RegsiterClassEx failed",
			L"HyperMandel",
			NULL);
		
		return 1;
	}
	
	// create window rect and adjust size to paintable area only
	
	RECT windowRect = { 0 };
	windowRect.right = WINDOW_INITIAL_RENDER_WIDTH + UI_WIDTH;
	windowRect.bottom = WINDOW_INITIAL_RENDER_HEIGHT;
	
#ifdef _DEBUG
	PRINT_DEBUG_4ARG(L"AdjustWindowRectEx: Unadjusted Rect: ", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
#endif
	
	AdjustWindowRectEx(
		&windowRect,
		WS_OVERLAPPEDWINDOW,
		FALSE,
		WS_EX_OVERLAPPEDWINDOW
	);
	
#ifdef _DEBUG
	PRINT_DEBUG_4ARG(L"AdjustWindowRectEx: Adjusted Rect: ", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
#endif
	
	// create window
	
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	
	// check if window created
	
	if (!hWnd) {
		MessageBox(NULL,
			L"Call to CreateWindowEx failed",
			L"HyperMandel",
			NULL);
		
		return 1;
	}
	
	// assign main window handle
	mainHWnd = hWnd;
	
	// create ui
	
#define _PLACEELEM(UIVar, coordRef) {\
	std::wstring coordRefString = float_to_string(coordRef);\
	\
	UIVar.hWnd = CreateWindowEx(\
		WS_EX_CLIENTEDGE,\
		UIVar.windowClass,\
		coordRefString.c_str(),\
		WS_GROUP | WS_TABSTOP | WS_CHILD | WS_VISIBLE,\
		WINDOW_INITIAL_RENDER_WIDTH + UIVar.x, UIVar.y,\
		UIVar.w, UIVar.h,\
		hWnd,\
		NULL,\
		hInstance,\
		NULL);\
	\
	if (!UIVar.hWnd) {\
		WCHAR funcName[] = L"CreateWindowEx";\
		WCHAR desc[] = L###UIVar;\
		errorMsgBox(funcName, desc);\
		return 1;\
	}\
	\
	/* third param is uIDSubclass and according to windows example can be set to 0 (so basically null) */\
	SetWindowSubclass(UIVar.hWnd, EditProc, NULL, (DWORD_PTR)&coordRef);\
}
	
	_PLACEELEM(UIElems.Location.X, mandelCoords.cx);
	_PLACEELEM(UIElems.Location.Y, mandelCoords.cy);
	_PLACEELEM(UIElems.Location.Zoom, mandelCoords.zoom);
	
#undef _PLACEELEM
	
	// procure window
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// handle window messages
	
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int)msg.wParam;
}
