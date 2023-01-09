#include "wnd_proc.hpp"

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
		_RESIZEELEM(UIElems.Location.Rotation);
		
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
		
		if (windowSize.width > UI_WIDTH)
			WndProc_paint_mandel(hdc);
		
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
		_PLACETEXT(UIElems.Location.RotationText);
		
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
	case WM_LBUTTONDOWN:
		inputs.raw.mouseButtons.left = true;
		inputs.processed.mouseButtons.any = true;
		WndProc_mouse_click_or_move<mandel_var_t>();
		RESET_FOCUS_TO_MAIN();
		break;
	case WM_LBUTTONUP:
		inputs.raw.mouseButtons.left = false;
		_RECALC_MOUSEBUTTONS_ANY;
		WndProc_mouse_click_or_move<mandel_var_t>();
		break;
	case WM_MBUTTONDOWN:
		inputs.raw.mouseButtons.middle = true;
		inputs.processed.mouseButtons.any = true;
		WndProc_mouse_click_or_move<mandel_var_t>();
		RESET_FOCUS_TO_MAIN();
		break;
	case WM_MBUTTONUP:
		inputs.raw.mouseButtons.middle = false;
		_RECALC_MOUSEBUTTONS_ANY;
		WndProc_mouse_click_or_move<mandel_var_t>();
		break;
	case WM_RBUTTONDOWN:
		inputs.raw.mouseButtons.right = true;
		inputs.processed.mouseButtons.any = true;
		WndProc_mouse_click_or_move<mandel_var_t>();
		RESET_FOCUS_TO_MAIN();
		break;
	case WM_RBUTTONUP:
		inputs.raw.mouseButtons.right = false;
		_RECALC_MOUSEBUTTONS_ANY;
		WndProc_mouse_click_or_move<mandel_var_t>();
		break;
	case WM_XBUTTONDOWN:
		if (HIWORD(wParam) == XBUTTON1) {
			inputs.raw.mouseButtons.mouse4 = true;
			inputs.processed.mouseButtons.any = true;
			WndProc_mouse_click_or_move<mandel_var_t>();
		} else {
			inputs.raw.mouseButtons.mouse5 = true;
			inputs.processed.mouseButtons.any = true;
			WndProc_mouse_click_or_move<mandel_var_t>();
		}
		RESET_FOCUS_TO_MAIN();
		break;
	case WM_XBUTTONUP:
		if (HIWORD(wParam) == XBUTTON1) {
			inputs.raw.mouseButtons.mouse4 = false;
			_RECALC_MOUSEBUTTONS_ANY;
			WndProc_mouse_click_or_move<mandel_var_t>();
		} else {
			inputs.raw.mouseButtons.mouse5 = false;
			_RECALC_MOUSEBUTTONS_ANY;
			WndProc_mouse_click_or_move<mandel_var_t>();
		}
		break;
	
#undef _RECALC_MOUSEBUTTONS_ANY
	
	// track mouse position
	case WM_MOUSEMOVE:
		inputs.raw.mousePos = MAKEPOINTS(lParam); // hopefully doesn't change mousePos to a new address somehow
		
		WndProc_mouse_click_or_move<mandel_var_t>(); // this is necessary because MOUSEMOVE doesn't lead to SETCURSOR if mouse is being held and dragged (it is called otherwise)
		break;
	
	case WM_SETCURSOR: // hoping that this will get called immediately after WM_MOUSEMOVE, every time (as it has done previously)
		if ((HWND)wParam == hWnd) {
			if (LOWORD(lParam) == HTCLIENT) {
				WndProc_mouse_click_or_move<mandel_var_t>();
				
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
	
	// mousewheel can be used for zooming or rotating
	case WM_MOUSEWHEEL:
		if (inputs.processed.draggingFractal || inputs.raw.mousePos.x < renderSize.width) {
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			WORD keyState = GET_KEYSTATE_WPARAM(wParam);
			if (keyState & MK_SHIFT) {
				// rotation
				// it's not a horizontal scroll but the code called is the same
				if (keyState & MK_CONTROL)
					WndProc_mouse_wheel_horizontal<mandel_var_t>(zDelta, false); // fine rotation
				else
					WndProc_mouse_wheel_horizontal<mandel_var_t>(zDelta, true); // coarse rotation
			} else {
				// regular zooming in and out
				if (keyState & MK_CONTROL)
					WndProc_mouse_wheel<mandel_var_t>(zDelta, false); // fine zoom
				else
					WndProc_mouse_wheel<mandel_var_t>(zDelta, true); // coarse zoom
			}
		}
		break;
	
	// horizontal clicking of mousewheel used for rotation only
	case WM_MOUSEHWHEEL:
		if (inputs.processed.draggingFractal || inputs.raw.mousePos.x < renderSize.width) {
			short wDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			WORD keyState = GET_KEYSTATE_WPARAM(wParam);
			if (keyState & MK_SHIFT) {
				// zooming in and out
				if (keyState & MK_CONTROL)
					WndProc_mouse_wheel<mandel_var_t>(wDelta, false); // fine rotation
				else
					WndProc_mouse_wheel<mandel_var_t>(wDelta, true); // coarse rotation
			} else {
				// rotation
				if (keyState & MK_CONTROL)
					WndProc_mouse_wheel_horizontal<mandel_var_t>(wDelta, false); // fine zoom
				else
					WndProc_mouse_wheel_horizontal<mandel_var_t>(wDelta, true); // coarse zoom
			}
		}
		break;
	
	case WM_KEYDOWN:
		if (wParam >= VK_F13 && wParam <= VK_F24) {
			// f13 - f24
			MessageBox(hWnd, L"We don't do that here.", L"F13 to F24 Pressed", MB_OK);
			return FALSE; // processed the message
		}
		
		return DefWindowProc(hWnd, message, wParam, lParam); // did not process the message
		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0; // processed the message
}
