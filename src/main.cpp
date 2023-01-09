#include "main.hpp"

// declare the extern globals

UIElem_T UIElems;
HINSTANCE mainHInstance;
HWND mainHWnd;
WIDHEIGHT windowSize = { 0 };
WIDHEIGHT renderSize = { 0 };
mandel::calc::Coords<MANDEL_VAR_T> mandelCoords;
mandel::calc::Basic_MultiPixel_Args<MANDEL_VAR_T> mandelArgs = { 0 };
Inputs_T inputs;
LPWSTR currentDeliberateCursor = IDC_ARROW;

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
	ERROR_CHECK_ZERO(wcex.hIcon, L"LoadImage", L"WinMain/wcex.hIcon");
#else
	wcex.hIcon = (HICON)LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED); // loaded from a file but not explicitly (loaded from exe itself, specified in resource file), don't know if LR_SHARED is a problem
	ERROR_CHECK_ZERO(wcex.hIcon, L"LoadImage", L"WinMain/wcex.hIcon");
#endif
	wcex.hCursor = (HCURSOR)LoadImage(NULL, currentDeliberateCursor, IMAGE_CURSOR, 0, 0, LR_SHARED);
	ERROR_CHECK_ZERO(wcex.hCursor, L"LoadImage", L"WinMain/wcex.hCursor");
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) /* white */;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
#ifdef _NOICON
	wcex.hIconSm = (HICON)LoadImage(wcex.hInstance, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	ERROR_CHECK_ZERO(wcex.hIconSm, L"LoadImage", L"WinMain/wcex.hIconSm");
#else
	wcex.hIconSm = (HICON)LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED); // loaded from a file but not explicitly (loaded from exe itself, specified in resource file), don't know if LR_SHARED is a problem
	ERROR_CHECK_ZERO(wcex.hIconSm, L"LoadImage", L"WinMain/wcex.hIconSm");
#endif
	
	// register window class
	
	ERROR_CHECK_ZERO_EXTRA(
		RegisterClassEx(&wcex),
		L"RegisterClassEx", L"WinMain",
		return 1);
	
	// create window rect with client area and adjust size to window area
	
	RECT windowRect = { 0 };
	windowRect.right = WINDOW_INITIAL_RENDER_WIDTH + UI_WIDTH;
	windowRect.bottom = WINDOW_INITIAL_RENDER_HEIGHT;
	
#ifdef _DEBUG
	PRINT_DEBUG_4ARG(L"AdjustWindowRectEx: Unadjusted Rect: ", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
#endif
	
	ERROR_CHECK_ZERO(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW),
		L"AdjustWindowRectEx", L"WinMain"
	);
	
#ifdef _DEBUG
	PRINT_DEBUG_4ARG(L"AdjustWindowRectEx: Adjusted Rect: ", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
#endif
	
	// create window and check if created
	
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
	ERROR_CHECK_ZERO_EXTRA(hWnd, L"CreateWindowEx", L"WinMain", return 1);
	
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
	ERROR_CHECK_ZERO_EXTRA(UIVar.hWnd, L"CreateWindowEx", L"WinMain/" L###UIVar, return 1);\
	\
	ERROR_CHECK_ZERO_EXTRA(\
		/* third param is uIDSubclass and according to windows example can be set to 0 (so basically null) */\
		SetWindowSubclass(UIVar.hWnd, EditProc, NULL, (DWORD_PTR)&coordRef),\
		L"SetWindowSubclass", L"WinMain/" L###UIVar,\
		return 1);\
}
	
	_PLACEELEM(UIElems.Location.X, mandelCoords.cx);
	_PLACEELEM(UIElems.Location.Y, mandelCoords.cy);
	_PLACEELEM(UIElems.Location.Zoom, mandelCoords.zoom)
	_PLACEELEM(UIElems.Location.Rotation, mandelCoords.rotation);
	
#undef _PLACEELEM
	
	// procure window
	
	WARN_CHECK_NONZERO(ShowWindow(hWnd, nCmdShow), L"UpdateWindow", L"WinMain", L"window already visible");
	ERROR_CHECK_ZERO(UpdateWindow(hWnd), L"UpdateWindow", L"WinMain");
	
	// handle window messages
	
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg); // ignoring error checking because docs confusing
		DispatchMessage(&msg); // ignoring return value
	}
	
	return (int)msg.wParam;
}
