#pragma once

#include <sstream>

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include "consts.h"
#include "defines.h"
#include "structs.h"
#include "globals.h"
#include "misc.h"
#include "wnd_proc_paint_mandel.h"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

LRESULT CALLBACK EditProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR drRefData
);
