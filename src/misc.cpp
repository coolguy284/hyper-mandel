#include "misc.h"

std::wstring get_string_from_editctrl(HWND hWnd) {
	int editContentsLength = GetWindowTextLength(hWnd) + 1;
	
	std::unique_ptr<wchar_t[]> editContents = std::make_unique<wchar_t[]>(editContentsLength);
	
	GetWindowText(hWnd, editContents.get(), editContentsLength);
	
	return std::wstring(editContents.get());
}

std::wstring float_to_string(float val) {
	std::wostringstream strVal;
	
	strVal << val;
	
	return strVal.str();
}
