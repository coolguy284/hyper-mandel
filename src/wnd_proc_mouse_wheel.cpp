#include "wnd_proc_mouse_wheel.h"

void WndProc_mouse_wheel(short zDelta) {
	if (zDelta != 0) {
		// special math with current x and y coordinates so that zoom is relative to the cross's position
		
		float zoomFactor = powf(SCROLL_ZOOM_MULT, zDelta);
		
		float cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		float cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		float cxDiff = cxCursor - mandelCoords.cx;
		float cyDiff = cyCursor - mandelCoords.cy;
		
		float cxZoomDiff = cxDiff - cxDiff / zoomFactor;
		float cyZoomDiff = cyDiff - cyDiff / zoomFactor;
		
		// apply the calculated zoom and set textboxes
		
		mandelCoords.zoom *= zoomFactor;
		
		float newCx = mandelCoords.cx + cxZoomDiff;
		float newCy = mandelCoords.cy + cyZoomDiff;
		
		SET_TEXTBOX_TEXT(UIElems.Location.Zoom, mandelCoords.zoom);
		
		if (newCx != mandelCoords.cx) {
			mandelCoords.cx = newCx;
			SET_TEXTBOX_TEXT(UIElems.Location.X, mandelCoords.cx);
		}
		
		if (newCy != mandelCoords.cy) {
			mandelCoords.cy = newCy;
			SET_TEXTBOX_TEXT(UIElems.Location.Y, mandelCoords.cy);
		}
		
		WndProc_paint_mandel();
	}
}

void WndProc_mouse_wheel_horizontal(short wDelta, bool coarseScrolling) {
	if (wDelta != 0) {
		if (coarseScrolling)
			mandelCoords.rotation += wDelta * HSCROLL_COARSE_ROTATION_MULT;
		else
			mandelCoords.rotation += wDelta * HSCROLL_FINE_ROTATION_MULT;
		
		SET_TEXTBOX_TEXT(UIElems.Location.Rotation, mandelCoords.rotation);
		
		WndProc_paint_mandel();
	}
}
