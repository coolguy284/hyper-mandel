#pragma once

#define VERSION L"v1.0.0"

constexpr int WINDOW_INITIAL_RENDER_HEIGHT = 480;
constexpr int WINDOW_INITIAL_RENDER_WIDTH = (int)(WINDOW_INITIAL_RENDER_HEIGHT * (16.0f / 9.0f)); // perfect 16:9 conversion, currently 853
constexpr int UI_WIDTH = 200;

// a normal unit of scroll is this to the 120th power.
constexpr float SCROLL_ZOOM_MULT = 1.0033845907368393f; // powf(1.5f, 1.0f / 120.0f) is not constexpr so it's evaluated value is here instead

// a normal unit of hscroll are these times 120
constexpr float HSCROLL_COARSE_ROTATION_MULT = 15.0f / 120.0f;
constexpr float HSCROLL_FINE_ROTATION_MULT = 0.5f / 120.0f;

enum class RENDER_MODES {
	FILLRECT,
	BITMAP_SETPIXEL,
	BITMAP_PIXELARRAY,
};

constexpr RENDER_MODES RENDER_MODE = RENDER_MODES::BITMAP_PIXELARRAY;
