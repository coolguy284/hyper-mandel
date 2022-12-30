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
	
	// set p variables
	inputs.raw.mouseButtons.pLeft = inputs.raw.mouseButtons.left;
	inputs.raw.mouseButtons.pMiddle = inputs.raw.mouseButtons.middle;
	inputs.raw.mouseButtons.pRight = inputs.raw.mouseButtons.right;
	inputs.raw.mouseButtons.pMouse4 = inputs.raw.mouseButtons.mouse4;
	inputs.raw.mouseButtons.pMouse5 = inputs.raw.mouseButtons.mouse5;
	
	inputs.raw.pMousePos = inputs.raw.mousePos;
	
	inputs.processed.mouseButtons.pAny = inputs.processed.mouseButtons.any;
}
