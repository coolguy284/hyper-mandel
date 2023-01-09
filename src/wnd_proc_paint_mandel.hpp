#pragma once

#include <memory>

#include "mandel/calculate.hpp"
#include "mandel/render.hpp"

#include "consts.hpp"
#include "structs_defines.hpp"
#include "globals.hpp"
#include "error_funcs_defines.hpp"
#include "debug_print_defines.hpp"

void WndProc_paint_mandel(HDC hdc);

bool WndProc_paint_mandel();
