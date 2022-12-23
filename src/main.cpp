#include <Windows.h>

#include "../resources/icon.h"

#include "consts.h"
#include "globals.h"
#include "wnd_proc.h"

// declare the extern globals

UIElem_T UIElems;
HINSTANCE mainHInstance;

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
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
#else
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
#endif
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) /* white */;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
#ifdef _NOICON
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
#else
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));
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
	
	// create ui
	
#define _PLACEELEM(UIVar) {\
	UIVar.hWnd = CreateWindowEx(\
		WS_EX_CLIENTEDGE,\
		UIVar.windowClass,\
		UIVar.title,\
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
}
	
	_PLACEELEM(UIElems.Location.X);
	_PLACEELEM(UIElems.Location.Y);
	_PLACEELEM(UIElems.Location.Zoom);
	
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
