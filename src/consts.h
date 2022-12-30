#pragma once

#define VERSION L"v0.2.1-alpha"

constexpr int WINDOW_INITIAL_RENDER_HEIGHT = 480;
constexpr int WINDOW_INITIAL_RENDER_WIDTH = (int)(WINDOW_INITIAL_RENDER_HEIGHT * (16.0 / 9.0)); // perfect 16:9 conversion, currently 853
constexpr int UI_WIDTH = 200;

enum class RENDER_MODES {
	FILLRECT,
	BITMAP_SETPIXEL,
	BITMAP_PIXELARRAY,
};

constexpr RENDER_MODES RENDER_MODE = RENDER_MODES::BITMAP_PIXELARRAY;
