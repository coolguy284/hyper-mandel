#pragma once

#include <Windows.h>
#include <sstream>

#include "consts.h"
#include "structs.h"
#include "wnd_proc_paint_mandel.h"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
