template <typename T>
LRESULT CALLBACK EditProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR drRefData
) {
	switch (message) {
	case WM_CHAR: {
		// recalculate mandelbrot set if enter key pressed
		if (wParam == VK_RETURN) {
			// get contents in edit
			std::wstring strValToSet = get_string_from_editctrl(hWnd);
			
			try {
				// get T version of string contents in edit
				T valToSet = std::stof(strValToSet);
				
				// set value referenced by drRefData (points to a T in mandelCoords)
				*(T*)drRefData = valToSet;
				
				// rerun paint_mandel
				WndProc_paint_mandel();
			} catch (const std::invalid_argument e) {
				MessageBox(NULL, L"Value is not a valid number.", L"HyperMandel Error", MB_OK);
			} catch (const std::out_of_range e) {
				MessageBox(NULL, L"Value is not in range of datatype.", L"HyperMandel Error", MB_OK);
			}
		} else {
			// handoff to default action
			return DefSubclassProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	
	/*case WM_NCDESTROY:
		// deliberately not removing subclass as windows example doesn't do it
		// https://learn.microsoft.com/en-us/windows/win32/controls/subclassing-overview
		//RemoveWindowSubclass(hWnd, EditProc, uIdSubclass);
		// deliberately not clearing drRefData as it points to struct in global memory
		break;*/
	
	default:
		return DefSubclassProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}
