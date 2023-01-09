#pragma once

#include "consts.hpp"
#include "globals.hpp"
#include "misc.hpp"
#include "error_funcs_defines.hpp"
#include "wnd_proc_paint_mandel.hpp"

void WndProc_mouse_wheel(short zDelta);

void WndProc_mouse_wheel_horizontal(short wDelta, bool coarseScrolling);
