#pragma once

#define VERSION L"v1.0.1"

constexpr int WINDOW_INITIAL_RENDER_HEIGHT = 480;
constexpr int WINDOW_INITIAL_RENDER_WIDTH = (int)(WINDOW_INITIAL_RENDER_HEIGHT * (16.0L / 9.0L)); // perfect 16:9 conversion, currently 853
constexpr int UI_WIDTH = 200;

// type used for all fractal calculations
using mandel_var_t = float;

// a normal unit of scroll is this to the 120th power.
constexpr mandel_var_t SCROLL_ZOOM_MULT = (mandel_var_t)1.0033845907368393L; // pow(1.5L, 1.0L / 120.0L) is not constexpr so it's evaluated value is here instead

// a normal unit of hscroll are these times 120
constexpr mandel_var_t HSCROLL_COARSE_ROTATION_MULT = (mandel_var_t)(15.0L / 120.0L);
constexpr mandel_var_t HSCROLL_FINE_ROTATION_MULT = (mandel_var_t)(0.5L / 120.0L);

enum class RENDER_MODES {
	FILLRECT,
	BITMAP_SETPIXEL,
	BITMAP_PIXELARRAY,
};

constexpr RENDER_MODES RENDER_MODE = RENDER_MODES::BITMAP_PIXELARRAY;
