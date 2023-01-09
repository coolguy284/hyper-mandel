#pragma once

#include "structs_defines.hpp"
#include "globals.hpp"
#include "misc.hpp"
#include "error_funcs_defines.hpp"
#include "debug_print_defines.hpp"
#include "wnd_proc_paint_mandel.hpp"

template <typename T>
void WndProc_mouse_click_or_move();

#include "wnd_proc_mouse_click_or_move.tpp"
