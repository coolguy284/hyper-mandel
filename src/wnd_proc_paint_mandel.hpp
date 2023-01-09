#pragma once

#include <memory>

#include "mandel/calculate.h"
#include "mandel/render.h"

#include "consts.h"
#include "structs_defines.h"
#include "globals.h"
#include "error_funcs_defines.h"
#include "debug_print_defines.h"

void WndProc_paint_mandel(HDC hdc);

bool WndProc_paint_mandel();
