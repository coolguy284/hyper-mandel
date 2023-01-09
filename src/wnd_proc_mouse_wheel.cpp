#include "wnd_proc_mouse_wheel.hpp"

void WndProc_mouse_wheel(short zDelta) {
	if (zDelta != 0) {
		// math with current x and y coordinates so that zoom is relative to the cross's position
		
		mandel_var_t zoomFactor = pow(SCROLL_ZOOM_MULT, zDelta);
		
		mandel_var_t cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		mandel_var_t cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		mandel_var_t cxDiff = cxCursor - mandelCoords.cx;
		mandel_var_t cyDiff = cyCursor - mandelCoords.cy;
		
		mandel_var_t cxZoomDiff = cxDiff - cxDiff / zoomFactor;
		mandel_var_t cyZoomDiff = cyDiff - cyDiff / zoomFactor;
		
		// apply calculated zoom and set textboxes
		
		mandelCoords.zoom *= zoomFactor;
		
		mandel_var_t newCx = mandelCoords.cx + cxZoomDiff;
		mandel_var_t newCy = mandelCoords.cy + cyZoomDiff;
		
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
		// math with current x and y coordinates so that rotate is relative to the cross's position
		
		mandel_var_t rotAmt = 0.0L;
		
		if (coarseScrolling)
			rotAmt = wDelta * HSCROLL_COARSE_ROTATION_MULT;
		else
			rotAmt = wDelta * HSCROLL_FINE_ROTATION_MULT;
		
		mandel_var_t rotAmtRad = rotAmt / (mandel_var_t)180.0L * std::numbers::pi_v<mandel_var_t>;
		
		mandel_var_t newRotation = mod_pos(mandelCoords.rotation + rotAmt, (mandel_var_t)360.0);
		
		mandel_var_t cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		mandel_var_t cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		// diffs flipped compared to the zoom version
		mandel_var_t cxDiff = mandelCoords.cx - cxCursor;
		mandel_var_t cyDiff = mandelCoords.cy - cyCursor;
		
		mandel_var_t rotCxDiff = cos(rotAmtRad) * cxDiff - sin(rotAmtRad) * cyDiff;
		mandel_var_t rotCyDiff = cos(rotAmtRad) * cyDiff + sin(rotAmtRad) * cxDiff;
		
		mandel_var_t newCx = rotCxDiff - cxDiff + mandelCoords.cx;
		mandel_var_t newCy = rotCyDiff - cyDiff + mandelCoords.cy;
		
		// apply calculated rotation and set textboxes
		
		mandelCoords.rotation = newRotation;
		
		SET_TEXTBOX_TEXT(UIElems.Location.Rotation, mandelCoords.rotation);
		
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
