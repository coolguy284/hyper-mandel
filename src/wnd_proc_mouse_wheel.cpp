#include "wnd_proc_mouse_wheel.hpp"

void WndProc_mouse_wheel(short zDelta) {
	if (zDelta != 0) {
		// math with current x and y coordinates so that zoom is relative to the cross's position
		
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
		
		// apply calculated zoom and set textboxes
		
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
		// math with current x and y coordinates so that rotate is relative to the cross's position
		
		float rotAmt = 0.0f;
		
		if (coarseScrolling)
			rotAmt = wDelta * HSCROLL_COARSE_ROTATION_MULT;
		else
			rotAmt = wDelta * HSCROLL_FINE_ROTATION_MULT;
		
		float rotAmtRad = rotAmt / 180.0f * std::numbers::pi_v<float>;
		
		float newRotation = mod_pos(mandelCoords.rotation + rotAmt, 360.0f);
		
		float cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		float cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		// diffs flipped compared to the zoom version
		float cxDiff = mandelCoords.cx - cxCursor;
		float cyDiff = mandelCoords.cy - cyCursor;
		
		float rotCxDiff = cosf(rotAmtRad) * cxDiff - sinf(rotAmtRad) * cyDiff;
		float rotCyDiff = cosf(rotAmtRad) * cyDiff + sinf(rotAmtRad) * cxDiff;
		
		float newCx = rotCxDiff - cxDiff + mandelCoords.cx;
		float newCy = rotCyDiff - cyDiff + mandelCoords.cy;
		
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
