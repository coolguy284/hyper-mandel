#include "wnd_proc_mouse_click_or_move.h"

void WndProc_mouse_click_or_move() {
	// if not clicked: change cursor to cross on left side (for fractal), arrow on right (for ui)
	// if clicked down on right: enter fractal dragging mode (cursor becomes 4 way arrow), capture pointer, only leave mode when let go
	if (!inputs.processed.draggingFractal) {
		if (!inputs.processed.mouseButtons.any) {
			if (inputs.raw.mousePos.x < renderSize.width) {
				SET_CURSOR(IDC_CROSS);
			} else {
				SET_CURSOR(IDC_ARROW);
			}
		} else {
			if (inputs.raw.mousePos.x < renderSize.width && !inputs.processed.mouseButtons.pAny) {
				inputs.processed.draggingFractal = true;
				SET_CURSOR(IDC_SIZEALL);
				SetCapture(mainHWnd);
			}
		}
	} else {
		if (!inputs.processed.mouseButtons.any) {
			inputs.processed.draggingFractal = false;
			WARN_CHECK_ZERO(
				ReleaseCapture(),
				L"ReleaseCapture", L"WndProc_mouse_click_or_move",
				L"mouse captured (i think) but releasecapture failed");
			// cursor doesn't need to be reset because it appears that this function gets called again immediately anyway
		}
	}
	
	// perform mandel dragging
	if (inputs.processed.draggingFractal) {
		bool updated = false;

		if (inputs.processed.mouseDelta.x) {
			mandelCoords.cx -= inputs.processed.mouseDelta.x * mandelArgs.cxStep;
			std::wstring cxString = float_to_string(mandelCoords.cx);
			ERROR_CHECK_ZERO(
				SetWindowText(UIElems.Location.X.hWnd, cxString.c_str()),
				L"SetWindowText", L"WndProc_mouse_click_or_move");

			updated = true;
		}

		if (inputs.processed.mouseDelta.y) {
			mandelCoords.cy -= inputs.processed.mouseDelta.y * mandelArgs.cyStep;
			std::wstring cyString = float_to_string(mandelCoords.cy);
			ERROR_CHECK_ZERO(
				SetWindowText(UIElems.Location.Y.hWnd, cyString.c_str()),
				L"SetWindowText", L"WndProc_mouse_click_or_move");

			updated = true;
		}

		if (updated) {
			WndProc_paint_mandel();
		}
	}

	// calculate delta
	inputs.processed.mouseDelta = {
		(short)(inputs.raw.mousePos.x - inputs.raw.pMousePos.x),
		(short)(inputs.raw.mousePos.y - inputs.raw.pMousePos.y)
	};

	// set p variables
	inputs.raw.mouseButtons.pLeft = inputs.raw.mouseButtons.left;
	inputs.raw.mouseButtons.pMiddle = inputs.raw.mouseButtons.middle;
	inputs.raw.mouseButtons.pRight = inputs.raw.mouseButtons.right;
	inputs.raw.mouseButtons.pMouse4 = inputs.raw.mouseButtons.mouse4;
	inputs.raw.mouseButtons.pMouse5 = inputs.raw.mouseButtons.mouse5;
	
	inputs.raw.pMousePos = inputs.raw.mousePos;
	
	inputs.processed.mouseButtons.pAny = inputs.processed.mouseButtons.any;
}
