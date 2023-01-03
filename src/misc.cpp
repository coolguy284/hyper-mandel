#include "misc.h"

std::wstring float_to_string(float val) {
	std::wostringstream strVal;
	
	strVal << val;
	
	return strVal.str();
}

std::wstring get_string_from_editctrl(HWND hWnd) {
	int editContentsLength = GetWindowTextLength(hWnd) + 1;
	
	std::unique_ptr<wchar_t[]> editContents = std::make_unique<wchar_t[]>(editContentsLength);
	
	GetWindowText(hWnd, editContents.get(), editContentsLength); // ignoring error checking because return value of zero could be error or could be length zero edit control
	
	return std::wstring(editContents.get());
}
