#pragma once

#include <Windows.h>

#include "mandel/calculate.h"

#include "consts.h"
#include "structs.h"

const WCHAR szWindowClass[] = L"DesktopApp"; // window class
const WCHAR szTitle[] = L"HyperMandel " VERSION; // window title

#define _UIELEMSTRUCT_CONSTCHAR(name, str) const WCHAR name[sizeof(str)/sizeof(WCHAR)] = str
#define _UIELEMSTRUCT_UIELEM(name, xv, yv, wv, hv, cls, initialVal) \
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
		_UIELEMSTRUCT_UIELEM(X, 10, 28, 180, 23, L"Edit", L"");
		
		_UIELEMSTRUCT_UITEXT(YText, 10, 60, L"Y:");
		_UIELEMSTRUCT_UIELEM(Y, 10, 78, 180, 23, L"Edit", L"");
		
		_UIELEMSTRUCT_UITEXT(ZoomText, 10, 110, L"Zoom:");
		_UIELEMSTRUCT_UIELEM(Zoom, 10, 128, 180, 23, L"Edit", L"");
	};

	Location_T Location;
};

#undef _UIELEMSTRUCT_CONSTCHAR
#undef _UIELEMSTRUCT_UIELEM
#undef _UIELEMSTRUCT_UITEXT

extern UIElem_T UIElems;
extern HINSTANCE mainHInstance;
extern HWND mainHWnd;
extern WIDHEIGHT windowSize;
extern WIDHEIGHT renderSize;
extern mandel::calc::Coords mandelCoords;
