#pragma once

#include <sstream>

#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>

#include "consts.hpp"
#include "structs_defines.hpp"
#include "globals.hpp"
#include "misc.hpp"
#include "error_funcs_defines.hpp"
#include "debug_print_defines.hpp"
#include "wnd_proc_paint_mandel.hpp"
#include "wnd_proc_mouse_click_or_move.hpp"
#include "wnd_proc_mouse_wheel.hpp"

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

template <typename T>
LRESULT CALLBACK EditProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR drRefData
);

#include "wnd_proc.tpp"
