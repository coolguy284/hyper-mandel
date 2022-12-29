#include "wnd_proc_paint_mandel.h"

void WndProc_paint_mandel(HDC hdc) {
	unsigned int mandelArrayLength = renderSize.width * renderSize.height;
	
	if (mandelArrayLength > 0) {
		// calculate mandelbrot set and coloring
		
		std::unique_ptr<int[]> iterCountArr = std::make_unique<int[]>(mandelArrayLength);
		
		mandelCoords.width = renderSize.width;
		mandelCoords.height = renderSize.height;
		
		mandel::calc::Basic_MultiPixel_Args mpArgs = mandel::calc::convert_coord_to_multipixel(mandelCoords);
		
		mandel::calc::basic_multipixel(mpArgs, iterCountArr.get());
		
		std::unique_ptr<COLORREF[]> colorRefArr = std::make_unique<COLORREF[]>(mandelArrayLength);
		
		mandel::render::convert_iterctarr_to_colorarr(iterCountArr.get(), colorRefArr.get(), mandelArrayLength);
		
		// different render mode options
		
		switch (RENDER_MODE) {
		case RENDER_MODES::FILLRECT: {
			// calls fillrect on every pixel, ungodly slow
			
			RECT fr = { 0 };
			HBRUSH br;
			
			for (size_t y = 0; y < renderSize.height; y++) {
				for (size_t x = 0; x < renderSize.width; x++) {
					size_t baseColorIndex = x + y * renderSize.width;
					
					if (baseColorIndex < mandelArrayLength) {
						// apparently baseColorIndex could go over somehow
						fr.left = x;
						fr.top = y;
						fr.right = x + 1;
						fr.bottom = y + 1;
						
						br = CreateSolidBrush(colorRefArr[baseColorIndex]);
						ERROR_CHECK_ZERO_EXTRA(br, L"CreateSolidBrush", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT", goto fillrect_switch_break);
						
						bool deferBreak = false;
						ERROR_CHECK_ZERO_EXTRA(
							FillRect(hdc, &fr, br),
							L"FillRect", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT",
							deferBreak = true);
						
						ERROR_CHECK_ZERO_EXTRA(
							DeleteObject(br),
							L"DeleteObject", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT",
							goto fillrect_switch_break);
						
						if (deferBreak) goto fillrect_switch_break;
					}
				}
			}
			
		fillrect_switch_break:
			break;
		}
		
		case RENDER_MODES::BITMAP_SETPIXEL: {
			// creates a bitmap, then calls SetPixel, then renders it, slow
			
			// https://stackoverflow.com/questions/1748470/how-to-draw-image-on-a-window
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);
			
			if (hBitmap == 0) {
				errorMsgBox_const(L"CreateCompatibleBitmap", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
			} else {
				HDC hdcMem = CreateCompatibleDC(hdc);
				
				if (hdcMem == 0) {
					errorMsgBox_const(L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
				} else {
					HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
					
					if (oldBitmap == 0) {
						errorMsgBox_const(L"SelectObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
					} else {
						// setting pixels
						
						bool setPixelSuccess = true;
						
						size_t y, x;
						for (y = 0; y < renderSize.height; y++) {
							for (x = 0; x < renderSize.width; x++) {
								size_t baseColorIndex = x + y * renderSize.width;
								
								if (baseColorIndex < mandelArrayLength) {
									// apparently baseColorIndex could go over somehow
									ERROR_CHECK_NONZERO_EXTRA(
										SetPixel(hdcMem, x, y, colorRefArr[baseColorIndex]) == -1,
										L"SetPixel", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
										setPixelSuccess = false; goto setpixel_outer_for_break);
								}
							}
						}
						
					setpixel_outer_for_break:
						
						if (setPixelSuccess || y > 0 || x > 0) {
							// rendering bitmap to screen
							
							BITMAP bitmap = { 0 };
							GetObject(hBitmap, sizeof(bitmap), &bitmap);
							ERROR_CHECK_ZERO(
								BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY),
								L"BitBlt", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
						}
						
						// cleaning up
						
						ERROR_CHECK_ZERO(
							SelectObject(hdcMem, oldBitmap),
							L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
					}
					
					WARN_CHECK_ZERO(
						DeleteDC(hdcMem),
						L"DeleteDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
						L"device context not deleted");
				}
				
				WARN_CHECK_ZERO(
					DeleteObject(hBitmap),
					L"DeleteObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
					L"hBitmap not deleted");
			}
			break;
		}
		
		case RENDER_MODES::BITMAP_PIXELARRAY: {
			// creates a bitmap, then locks it and edits pixel memory, then renders it, decent,
			// but still slower than calculating the mandelbrot set itself -.-
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);
			
			if (hBitmap == 0) {
				errorMsgBox_const(L"CreateCompatibleBitmap", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
			} else {
				HDC hdcMem = CreateCompatibleDC(hdc);
				
				if (hdcMem == 0) {
					errorMsgBox_const(L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
				} else {
					HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
					
					if (oldBitmap == 0) {
						errorMsgBox_const(L"SelectObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
					} else {
						// https://stackoverflow.com/questions/3688409/getdibits-and-loop-through-pixels-using-x-y
						
						// getting pixel header data
						
						BITMAPINFO hBitmapInfo = { 0 };
						hBitmapInfo.bmiHeader.biSize = sizeof(hBitmapInfo.bmiHeader);
						
						if (GetDIBits(hdcMem, hBitmap, 0, 0, NULL, &hBitmapInfo, DIB_RGB_COLORS) == 0) {
							errorMsgBox_const(L"GetDIBits", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
						} else {
							std::unique_ptr<BYTE[]> lpPixels = std::make_unique<BYTE[]>(hBitmapInfo.bmiHeader.biSizeImage);
							hBitmapInfo.bmiHeader.biBitCount = 32;
							hBitmapInfo.bmiHeader.biCompression = BI_RGB;
							hBitmapInfo.bmiHeader.biHeight = abs(hBitmapInfo.bmiHeader.biHeight);
							
							bool setDIBitsSuccess = false;
							
							if (hBitmapInfo.bmiHeader.biSizeImage > 0) {
								// getting pixel data unnecessary because it will be fully reset anyway
								
								/*if (GetDIBits(hdcMem, hBitmap, 0, hBitmapInfo.bmiHeader.biHeight, lpPixels, &hBitmapInfo, DIB_RGB_COLORS) == 0) {
									errorMsgBox_const(L"GetDIBits 2", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
									break; // IF UNCOMMENTING REMOVE THIS BREAK AND MAKE THE STUFF BELOW GO IN AN ELSE
								}*/
								
								// setting pixels
								
								for (size_t y = 0; y < renderSize.height; y++) {
									for (size_t x = 0; x < renderSize.width; x++) {
										size_t basePixelIndex = x * 4 + y * renderSize.width * 4;
										
										if (basePixelIndex < hBitmapInfo.bmiHeader.biSizeImage) {
											// apparently basePixelIndex could go over somehow, not for colorRefArr like the others but lpPixels instead
											
											// capping colorRefIndex just in case
											size_t colorRefIndex = x + (renderSize.height - y - 1) * renderSize.width;
											
											if (colorRefIndex < mandelArrayLength) {
												COLORREF color = colorRefArr[colorRefIndex];
												lpPixels[basePixelIndex + 2] = GetRValue(color);
												lpPixels[basePixelIndex + 1] = GetGValue(color);
												lpPixels[basePixelIndex] = GetBValue(color);
											}
										}
									}
								}
								
								// copying edited pixel data back to bitmap
								
								if (SetDIBits(hdcMem, hBitmap, 0, hBitmapInfo.bmiHeader.biHeight, lpPixels.get(), &hBitmapInfo, DIB_RGB_COLORS) == 0) {
									errorMsgBox_const(L"SetDIBits", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
								} else {
									setDIBitsSuccess = true;
								}
							}
							
							// rendering bitmap to screen
							
							if (setDIBitsSuccess) {
								BITMAP bitmap = { 0 };
								GetObject(hBitmap, sizeof(bitmap), &bitmap);
								ERROR_CHECK_ZERO(
									BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY),
									L"BitBlt", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY");
							}
						}
						
						// cleaning up
						
						ERROR_CHECK_ZERO(
							SelectObject(hdcMem, oldBitmap),
							L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
					}
					
					WARN_CHECK_ZERO(
						DeleteDC(hdcMem),
						L"DeleteDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
						L"device context not deleted");
				}
				
				WARN_CHECK_ZERO(
					DeleteObject(hBitmap),
					L"DeleteObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
					L"hBitmap not deleted");
			}
			break;
		}
		}
	}
}
