#pragma once

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#include <Windows.h>

// https://stackoverflow.com/questions/13794171/how-to-make-the-mod-of-a-negative-number-to-be-positive/50900650#50900650
template<class T>
T mod_pos(T num, T divisor);

std::wstring float_to_string(float val);

std::wstring get_string_from_editctrl(HWND hWnd);

#include "misc.tpp"
