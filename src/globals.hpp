#pragma once

#include <Windows.h>

#include "mandel/calculate.hpp"

#include "consts.hpp"
#include "structs_defines.hpp"

const WCHAR szWindowClass[] = L"DesktopApp"; // window class
const WCHAR szTitle[] = L"HyperMandel " VERSION; // window title

#define _UIELEMSTRUCT_CONSTCHAR(name, str) const WCHAR name[sizeof(str)/sizeof(WCHAR)] = str
#define _UIELEMSTRUCT_UIELEM(name, xv, yv, wv, hv, cls) \
	struct _##name {\
		_UIELEMSTRUCT_CONSTCHAR(windowClass, cls);\
		int x = xv;\
		int y = yv;\
		int w = wv;\
		int h = hv;\
		HWND hWnd = { 0 };\
	};\
	_##name name

#define _UIELEMSTRUCT_UITEXT(name, xv, yv, textv) \
	struct _##name {\
		_UIELEMSTRUCT_CONSTCHAR(text, textv);\
		int x = xv;\
		int y = yv;\
	};\
	_##name name

struct UIElem_T {
	struct Location_T {
		_UIELEMSTRUCT_UITEXT(XText, 10, 10, L"X:");
		_UIELEMSTRUCT_UIELEM(X, 10, 28, 180, 23, L"Edit");
		
		_UIELEMSTRUCT_UITEXT(YText, 10, 60, L"Y:");
		_UIELEMSTRUCT_UIELEM(Y, 10, 78, 180, 23, L"Edit");
		
		_UIELEMSTRUCT_UITEXT(ZoomText, 10, 110, L"Zoom:");
		_UIELEMSTRUCT_UIELEM(Zoom, 10, 128, 180, 23, L"Edit");
		
		_UIELEMSTRUCT_UITEXT(RotationText, 10, 160, L"Rotation:");
		_UIELEMSTRUCT_UIELEM(Rotation, 10, 178, 180, 23, L"Edit");
	};
	
	Location_T Location;
};

#undef _UIELEMSTRUCT_CONSTCHAR
#undef _UIELEMSTRUCT_UIELEM
#undef _UIELEMSTRUCT_UITEXT

struct Inputs_T {
	struct Raw_T {
		struct MouseButtons_T {
			bool left = false;
			bool middle = false;
			bool right = false;
			bool mouse4 = false;
			bool mouse5 = false;
			
			bool pLeft = false;
			bool pMiddle = false;
			bool pRight = false;
			bool pMouse4 = false;
			bool pMouse5 = false;
		};
		
		MouseButtons_T mouseButtons;
		
		POINTS mousePos = { 0 };
		POINTS pMousePos = { 0 };
	};
	
	Raw_T raw;
	
	struct Processed_T {
		struct MouseButtons_T {
			bool any = false;
			bool pAny = false;
		};
		
		MouseButtons_T mouseButtons;
		
		bool draggingFractal = false;
		
		POINTS mouseDelta = { 0 };
	};
	
	Processed_T processed;
};

extern UIElem_T UIElems;
extern HINSTANCE mainHInstance;
extern HWND mainHWnd;
extern WIDHEIGHT windowSize;
extern WIDHEIGHT renderSize;
extern mandel::calc::Coords<mandel_var_t> mandelCoords;
extern mandel::calc::Basic_MultiPixel_Args<mandel_var_t> mandelArgs;
extern Inputs_T inputs;
extern LPWSTR currentDeliberateCursor;
