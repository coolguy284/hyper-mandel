#include "wnd_proc.h"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	// handles window events sent by windows os

	switch (message) {
	case WM_ERASEBKGND:
		// ignore this event because erasing background causes white flickering when resizing which is so bad it's seizure inducing
		return 1;

	case WM_PAINT: {
		// called when window needs to be updated (repainted)

		RECT winRect;

		if (!GetWindowRect(hWnd, &winRect)) {
			MessageBox(NULL,
				L"Call to GetWindowRect failed",
				L"HyperMandel",
				NULL);
			break;
		}

		// get size of paintable area inside window

		WIDHEIGHT windowSize;

		windowSize.width = max(winRect.right - winRect.left - WINDOW_EXTRA_WIDTH, 0); // dropshadow doesn't count
		windowSize.height = max(winRect.bottom - winRect.top - WINDOW_EXTRA_HEIGHT, 0); // title bar and dropshadow doesn't count

#ifdef _DEBUG

		std::wostringstream paintDebugOutputStream;

		paintDebugOutputStream << L"WM_PAINT: Window Size: ";
		paintDebugOutputStream << windowSize.width;
		paintDebugOutputStream << L", ";
		paintDebugOutputStream << windowSize.height;
		paintDebugOutputStream << L"\n";

		std::wstring paintDebugOutput = paintDebugOutputStream.str();

		OutputDebugString(paintDebugOutput.c_str());

#endif

		WIDHEIGHT renderSize;
		renderSize.width = max(windowSize.width - UI_WIDTH, 0); // UI_WIDTH less because removing right side for UI
		renderSize.height = windowSize.height;

		// painting

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// ignore everything but UI panel if window too small

		if (windowSize.width > UI_WIDTH) {
			WndProc_paint_mandel(renderSize, &hdc);
		}

		// paint ui on right (currently blank)

		RECT uiRect;
		uiRect.left = renderSize.width;
		uiRect.top = 0;
		uiRect.right = windowSize.width;
		uiRect.bottom = windowSize.height;

		FillRect(hdc, &uiRect, (HBRUSH)(COLOR_WINDOW + 1) /* white */);

		// paint finished

		EndPaint(hWnd, &ps);

		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
