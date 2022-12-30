#pragma once

#include <Windows.h>
#include <string>
#include <sstream>
#include <memory>

std::wstring get_string_from_editctrl(HWND hWnd);

std::wstring float_to_string(float val);
