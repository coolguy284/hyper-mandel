#pragma once

struct WIDHEIGHT {
	int width;
	int height;
};

// only make a windows api call if cursor needs changing
#define SET_CURSOR(cursor) \
	if (currentDeliberateCursor != cursor) {\
		SetCursor((HCURSOR)LoadImage(NULL, cursor, IMAGE_CURSOR, 0, 0, LR_SHARED));\
		currentDeliberateCursor = cursor;\
	}

// if cursor is set by another routine
#define RESET_CURSOR_VAR() currentDeliberateCursor = NULL;

#define SET_TEXTBOX_TEXT(UIVar, value) {\
	std::wstring valueStr = float_to_string(value);\
	ERROR_CHECK_ZERO(\
		SetWindowText(UIVar.hWnd, valueStr.c_str()),\
		L"SetWindowText", L"SET_TEXTBOX_TEXT");\
}
