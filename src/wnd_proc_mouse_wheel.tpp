#include "wnd_proc_mouse_wheel.hpp"

template <typename T>
void WndProc_mouse_wheel(short zDelta, bool coarseAdjustment) {
	if (zDelta != 0) {
		// math with current x and y coordinates so that zoom is relative to the cross's position
		
		T zoomFactor = 0.0f;
		if (coarseAdjustment)
			zoomFactor = pow(SCROLL_COARSE_ZOOM_MULT, zDelta);
		else
			zoomFactor = pow(SCROLL_FINE_ZOOM_MULT, zDelta);
		
		T cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		T cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		T cxDiff = cxCursor - mandelCoords.cx;
		T cyDiff = cyCursor - mandelCoords.cy;
		
		T cxZoomDiff = cxDiff - cxDiff / zoomFactor;
		T cyZoomDiff = cyDiff - cyDiff / zoomFactor;
		
		// apply calculated zoom and set textboxes
		
		mandelCoords.zoom *= zoomFactor;
		
		T newCx = mandelCoords.cx + cxZoomDiff;
		T newCy = mandelCoords.cy + cyZoomDiff;
		
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

template <typename T>
void WndProc_mouse_wheel_horizontal(short wDelta, bool coarseAdjustment) {
	if (wDelta != 0) {
		// math with current x and y coordinates so that rotate is relative to the cross's position
		
		T rotAmt = 0.0L;
		if (coarseAdjustment)
			rotAmt = wDelta * HSCROLL_COARSE_ROTATION_MULT;
		else
			rotAmt = wDelta * HSCROLL_FINE_ROTATION_MULT;
		
		T rotAmtRad = rotAmt / (T)180.0L * std::numbers::pi_v<T>;
		
		T newRotation = mod_pos(mandelCoords.rotation + rotAmt, (T)360.0);
		
		T cxCursor = mandelArgs.start_cx + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cx + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cx;
		T cyCursor = mandelArgs.start_cy + \
			inputs.raw.mousePos.x * mandelArgs.x_step_cy + \
			inputs.raw.mousePos.y * mandelArgs.y_step_cy;
		
		// diffs flipped compared to the zoom version
		T cxDiff = mandelCoords.cx - cxCursor;
		T cyDiff = mandelCoords.cy - cyCursor;
		
		T rotCxDiff = cos(rotAmtRad) * cxDiff - sin(rotAmtRad) * cyDiff;
		T rotCyDiff = cos(rotAmtRad) * cyDiff + sin(rotAmtRad) * cxDiff;
		
		T newCx = rotCxDiff - cxDiff + mandelCoords.cx;
		T newCy = rotCyDiff - cyDiff + mandelCoords.cy;
		
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
