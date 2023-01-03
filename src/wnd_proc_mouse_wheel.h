#pragma once

#include "consts.h"
#include "globals.h"
#include "misc.h"
#include "error_funcs_defines.h"
#include "wnd_proc_paint_mandel.h"

void WndProc_mouse_wheel(short zDelta);

void WndProc_mouse_wheel_horizontal(short wDelta, bool coarseScrolling);
