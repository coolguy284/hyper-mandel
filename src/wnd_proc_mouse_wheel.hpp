#pragma once

#include "consts.hpp"
#include "globals.hpp"
#include "misc.hpp"
#include "error_funcs_defines.hpp"
#include "wnd_proc_paint_mandel.hpp"

template <typename T>
void WndProc_mouse_wheel(short zDelta, bool coarseAdjustment);

template <typename T>
void WndProc_mouse_wheel_horizontal(short wDelta, bool coarseAdjustment);

#include "wnd_proc_mouse_wheel.tpp"
