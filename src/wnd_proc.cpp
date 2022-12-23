#include "wnd_proc.h"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	// handles window events sent by windows os
	
	switch (message) {
	case WM_SIZE: {
		WIDHEIGHT windowSize = { 0 };
		windowSize.width = max(LOWORD(lParam), 0);
		windowSize.height = max(HIWORD(lParam), 0);
		
		WIDHEIGHT renderSize = { 0 };
		renderSize.width = max(windowSize.width - UI_WIDTH, 0); // UI_WIDTH less because removing right side for UI
		renderSize.height = windowSize.height;
		
#define _RESIZEELEM(UIVar) \
	ERROR_WRAP(\
		SetWindowPos(\
			UIVar.hWnd,\
			NULL,\
			renderSize.width + UIVar.x, UIVar.y,\
			UIVar.w, UIVar.h,\
			NULL),\
		L"SetWindowPos", L###UIVar\
	);
		
		_RESIZEELEM(UIElems.Location.X);
		_RESIZEELEM(UIElems.Location.Y);
		_RESIZEELEM(UIElems.Location.Zoom);
		
#undef _RESIZEELEM
		break;
	}
	
	case WM_ERASEBKGND:
		// ignore this event because erasing background causes white flickering when resizing which is so bad it's seizure inducing
		return 1;
	
	case WM_PAINT: {
		// called when window needs to be updated (repainted)
		
		RECT winRect = { 0 };
		
		if (!GetClientRect(hWnd, &winRect)) {
			MessageBox(NULL,
				L"Call to GetClientRect failed",
				L"HyperMandel",
				NULL);
			break;
		}
		
		// get size of client area inside window
		
		WIDHEIGHT windowSize = { 0 };
		windowSize.width = max(winRect.right - winRect.left, 0);
		windowSize.height = max(winRect.bottom - winRect.top, 0);
		
#ifdef _DEBUG
		PRINT_DEBUG_2ARG(L"WM_PAINT: Window Size: ", windowSize.width, windowSize.height);
#endif
		
		WIDHEIGHT renderSize = { 0 };
		renderSize.width = max(windowSize.width - UI_WIDTH, 0); // UI_WIDTH less because removing right side for UI
		renderSize.height = windowSize.height;
		
		// painting
		
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hWnd, &ps);
		
		// ignore everything but UI panel if window too small
		
		if (windowSize.width > UI_WIDTH) {
			WndProc_paint_mandel(renderSize, &hdc);
		}
		
		// paint ui on right (currently blank)
		
		RECT uiRect = { 0 };
		uiRect.left = renderSize.width;
		uiRect.top = 0;
		uiRect.right = windowSize.width;
		uiRect.bottom = windowSize.height;
		
		FillRect(hdc, &uiRect, (HBRUSH)(COLOR_WINDOW) /* window background */);
		
#define _PLACETEXT(UITextVar) TextOut(hdc, renderSize.width + UITextVar.x, UITextVar.y, UITextVar.text, (int)wcslen(UITextVar.text))
		
		SetBkMode(hdc, TRANSPARENT);
		
		_PLACETEXT(UIElems.Location.XText);
		_PLACETEXT(UIElems.Location.YText);
		_PLACETEXT(UIElems.Location.ZoomText);
		
#undef _PLACETEXT
		
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
