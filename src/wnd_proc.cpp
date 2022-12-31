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
		windowSize.width = max(LOWORD(lParam), 0);
		windowSize.height = max(HIWORD(lParam), 0);
		
		renderSize.width = max(windowSize.width - UI_WIDTH, 0); // UI_WIDTH less because removing right side for UI
		renderSize.height = windowSize.height;
		
#ifdef _DEBUG
		PRINT_DEBUG_2ARG(L"WM_SIZE: Render Size: ", renderSize.width, renderSize.height);
#endif
		
#define _RESIZEELEM(UIVar) \
	ERROR_CHECK_ZERO(\
		SetWindowPos(\
			UIVar.hWnd,\
			NULL,\
			renderSize.width + UIVar.x, UIVar.y,\
			UIVar.w, UIVar.h,\
			NULL),\
		L"SetWindowPos", L"WndProc/WM_SIZE/" L###UIVar\
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
		
		ERROR_CHECK_ZERO_EXTRA(
			GetClientRect(hWnd, &winRect),
			L"GetClientRect", L"WndProc/WM_PAINT",
			break);
		
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
		ERROR_CHECK_ZERO_EXTRA(hdc, L"BeginPaint", L"WndProc/WM_PAINT", break);
		
		// ignore everything but UI panel if window too small
		
		if (windowSize.width > UI_WIDTH) {
			WndProc_paint_mandel(hdc);
		}
		
		// paint ui on right (currently blank)
		
		RECT uiRect = { 0 };
		uiRect.left = renderSize.width;
		uiRect.top = 0;
		uiRect.right = windowSize.width;
		uiRect.bottom = windowSize.height;
		
		ERROR_CHECK_ZERO(
			FillRect(hdc, &uiRect, (HBRUSH)(COLOR_WINDOW) /* window background */),
			L"FillRect", L"WndProc/WM_PAINT");
		
#define _PLACETEXT(UITextVar) \
	ERROR_CHECK_ZERO(\
		TextOut(hdc, renderSize.width + UITextVar.x, UITextVar.y, UITextVar.text, (int)wcslen(UITextVar.text)),\
		L"TextOut", L"WndProc/WM_PAINT")
		
		ERROR_CHECK_ZERO(
			SetBkMode(hdc, TRANSPARENT),
			L"SetBkMode", L"WndProc/WM_PAINT");
		
		_PLACETEXT(UIElems.Location.XText);
		_PLACETEXT(UIElems.Location.YText);
		_PLACETEXT(UIElems.Location.ZoomText);
		
#undef _PLACETEXT
		
		// paint finished
		
		EndPaint(hWnd, &ps);
		
		break;
	}
	
#define _RECALC_MOUSEBUTTONS_ANY \
	inputs.processed.mouseButtons.any = \
		inputs.raw.mouseButtons.left ||\
		inputs.raw.mouseButtons.middle ||\
		inputs.raw.mouseButtons.right ||\
		inputs.raw.mouseButtons.mouse4 ||\
		inputs.raw.mouseButtons.mouse5;
	
	// track mouse buttons
	case WM_LBUTTONDOWN:                  inputs.raw.mouseButtons.left = true;    inputs.processed.mouseButtons.any = true; WndProc_mouse_click_or_move(); break;
	case WM_LBUTTONUP:                    inputs.raw.mouseButtons.left = false;   _RECALC_MOUSEBUTTONS_ANY;                 WndProc_mouse_click_or_move(); break;
	case WM_MBUTTONDOWN:                  inputs.raw.mouseButtons.middle = true;  inputs.processed.mouseButtons.any = true; WndProc_mouse_click_or_move(); break;
	case WM_MBUTTONUP:                    inputs.raw.mouseButtons.middle = false; _RECALC_MOUSEBUTTONS_ANY;                 WndProc_mouse_click_or_move(); break;
	case WM_RBUTTONDOWN:                  inputs.raw.mouseButtons.right = true;   inputs.processed.mouseButtons.any = true; WndProc_mouse_click_or_move(); break;
	case WM_RBUTTONUP:                    inputs.raw.mouseButtons.right = false;  _RECALC_MOUSEBUTTONS_ANY;                 WndProc_mouse_click_or_move(); break;
	case WM_XBUTTONDOWN:
		if (HIWORD(wParam) == XBUTTON1) { inputs.raw.mouseButtons.mouse4 = true;  inputs.processed.mouseButtons.any = true; WndProc_mouse_click_or_move(); }
		else                            { inputs.raw.mouseButtons.mouse5 = true;  inputs.processed.mouseButtons.any = true; WndProc_mouse_click_or_move(); }
		break;
	case WM_XBUTTONUP:
		if (HIWORD(wParam) == XBUTTON1) { inputs.raw.mouseButtons.mouse4 = true;  _RECALC_MOUSEBUTTONS_ANY;                 WndProc_mouse_click_or_move(); }
		else                            { inputs.raw.mouseButtons.mouse5 = true;  _RECALC_MOUSEBUTTONS_ANY;                 WndProc_mouse_click_or_move(); }
		break;
	
#undef _RECALC_MOUSEBUTTONS_ANY
	
	// track mouse position
	case WM_MOUSEMOVE:
		inputs.raw.mousePos = MAKEPOINTS(lParam); // hopefully doesn't change mousePos to a new address somehow
		
		WndProc_mouse_click_or_move();
		break;
	
	case WM_SETCURSOR: // hoping that this will get called immediately after WM_MOUSEMOVE, every time (as it has done previously)
		if ((HWND)wParam == hWnd) {
			if (LOWORD(lParam) == HTCLIENT) {
				WndProc_mouse_click_or_move();
				
				// halt further processing
				return TRUE;
			} else {
				// run default processing (to set cursor to arrow or resizer, etc.)
				RESET_CURSOR_VAR();
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		} else {
			// allow further processing
			return FALSE;
		}
		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

LRESULT CALLBACK EditProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR drRefData
) {
	switch (message) {
	case WM_CHAR: {
		// recalculate mandelbrot set if enter key pressed
		if (wParam == VK_RETURN) {
			// get contents in edit
			std::wstring strValToSet = get_string_from_editctrl(hWnd);
			
			try {
				// get float version of string contents in edit
				float valToSet = std::stof(strValToSet);
				
				// set value referenced by drRefData (points to a float in mandelCoords)
				*(float*)drRefData = valToSet;
				
				// rerun paint_mandel
				WndProc_paint_mandel();
			} catch (const std::invalid_argument e) {
				MessageBox(NULL, L"Value is not a valid number.", L"HyperMandel Error", MB_OK);
			} catch (const std::out_of_range e) {
				MessageBox(NULL, L"Value is not in range of datatype.", L"HyperMandel Error", MB_OK);
			}
		} else {
			// handoff to default action
			return DefSubclassProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	
	/*case WM_NCDESTROY:
		// deliberately not removing subclass as windows example doesn't do it
		// https://learn.microsoft.com/en-us/windows/win32/controls/subclassing-overview
		//RemoveWindowSubclass(hWnd, EditProc, uIdSubclass);
		// deliberately not clearing drRefData as it points to struct in global memory
		break;*/
	
	default:
		return DefSubclassProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}
