#pragma once

#define VERSION L"v0.1.2-alpha"

// until something useful is placed in the ui box, set to 0 instead of 200
#define UI_WIDTH 0

// magic number for width or height of window's dropshadow
#define WINDOW_DROPSHADOW_SIZE 20

// magic number for height of title bar
#define TITLEBAR_HEIGHT 23

// consolidated magic numbers
#define WINDOW_EXTRA_WIDTH WINDOW_DROPSHADOW_SIZE
#define WINDOW_EXTRA_HEIGHT (TITLEBAR_HEIGHT + WINDOW_DROPSHADOW_SIZE)

enum {
	RENDER_MODE_FILLRECT,
	RENDER_MODE_BITMAP_SETPIXEL,
	RENDER_MODE_BITMAP_PIXELARRAY,
};

#define RENDER_MODE RENDER_MODE_BITMAP_PIXELARRAY
