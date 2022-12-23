#pragma once

#include <memory>

#include "mandel/calculate.h"
#include "mandel/render.h"

#include "consts.h"
#include "structs.h"

void WndProc_paint_mandel(const WIDHEIGHT renderSize, _In_ HDC* hdc);
