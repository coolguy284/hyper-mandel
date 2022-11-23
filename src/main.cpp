#define VERSION L"v0.0.3-alpha"

#include <Windows.h>

static WCHAR szWindowClass[] = L"DesktopApp"; // window "class" (will find out later)
static WCHAR szTitle[] = L"HyperMandel " VERSION; // window title

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	// handles window events sent by windows os

	PAINTSTRUCT ps;
	HDC hdc;
	WCHAR initialMessage[] = L"HyperMandel version " VERSION L".";

	switch (message) {
	case WM_PAINT:
		// called when window needs to be updated (repainted)
		
		hdc = BeginPaint(hWnd, &ps);

		TextOut(hdc,
			5, 5,
			initialMessage, (int)wcslen(initialMessage));

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	// create window class

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	// register window class

	if (!RegisterClassEx(&wcex)) {
		// register failure

		MessageBox(NULL,
			L"Call to RegsiterClassEx failed",
			L"HyperMandel",
			NULL);

		return 1;
	}

	// create window
	
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		840, 480,
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

	// procure window

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// handle window messages

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
