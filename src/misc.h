#pragma once

#include <memory>
#include <sstream>
#include <string>

#include <Windows.h>

std::wstring get_string_from_editctrl(HWND hWnd);

std::wstring float_to_string(float val);
