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
			
			for (int y = 0; y < renderSize.height; y++) {
				for (int x = 0; x < renderSize.width; x++) {
					unsigned int baseColorIndex = x + y * renderSize.width;
					
					if (baseColorIndex < mandelArrayLength) {
						// apparently baseColorIndex could go over somehow
						fr.left = x;
						fr.top = y;
						fr.right = x + 1;
						fr.bottom = y + 1;
						
						br = CreateSolidBrush(colorRefArr[baseColorIndex]);
						ERROR_CHECK_ZERO_EXTRA(br, L"CreateSolidBrush", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT", goto fillrect_switchbreak);
						
						bool deferBreak = false;
						ERROR_CHECK_ZERO_EXTRA(
							FillRect(hdc, &fr, br),
							L"FillRect", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT",
							deferBreak = true);
						
						ERROR_CHECK_ZERO_EXTRA(
							DeleteObject(br),
							L"DeleteObject", L"WndProc_paint_mandel/RENDER_MODES::FILLRECT",
							goto fillrect_switchbreak);
						
						if (deferBreak) goto fillrect_switchbreak;
					}
				}
			}
			
		fillrect_switchbreak:
			break;
		}
		
		case RENDER_MODES::BITMAP_SETPIXEL: {
			// creates a bitmap, then calls SetPixel, then renders it, slow
			
			// https://stackoverflow.com/questions/1748470/how-to-draw-image-on-a-window
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);
			ERROR_CHECK_ZERO_EXTRA(hBitmap, L"CreateCompatibleBitmap", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL", break);
			
			{
				HDC hdcMem = CreateCompatibleDC(hdc);
				ERROR_CHECK_ZERO_EXTRA(hdcMem, L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL", goto setpixel_DeleteObject);
				
				{
					HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
					ERROR_CHECK_NONZERO_EXTRA(!oldBitmap || oldBitmap == HGDI_ERROR, L"SelectObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL", goto setpixel_DeleteDC);
					
					// setting pixels
					
					for (int y = 0; y < renderSize.height; y++) {
						for (int x = 0; x < renderSize.width; x++) {
							unsigned int baseColorIndex = x + y * renderSize.width;
							
							if (baseColorIndex < mandelArrayLength) {
								// apparently baseColorIndex could go over somehow
								ERROR_CHECK_NONZERO_EXTRA(
									SetPixel(hdcMem, x, y, colorRefArr[baseColorIndex]) == -1,
									L"SetPixel", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL",
									goto setpixel_SelectObject);
							}
						}
					}
					
					{
						// rendering bitmap to screen
						
						BITMAP bitmap = { 0 };
						GetObject(hBitmap, sizeof(bitmap), &bitmap);
						ERROR_CHECK_ZERO(
							BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY),
							L"BitBlt", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
					}
				setpixel_SelectObject:
					// cleaning up
					
					ERROR_CHECK_ZERO(
						SelectObject(hdcMem, oldBitmap),
						L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_SETPIXEL");
				}
			setpixel_DeleteDC:
				DeleteDC(hdcMem);
			}
		setpixel_DeleteObject:
			DeleteObject(hBitmap);
			break;
		}
		
		case RENDER_MODES::BITMAP_PIXELARRAY: {
			// creates a bitmap, then locks it and edits pixel memory, then renders it, decent,
			// but still slower than calculating the mandelbrot set itself -.-
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);
			ERROR_CHECK_ZERO_EXTRA(hBitmap, L"CreateCompatibleBitmap", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY", break);
			
			{
				HDC hdcMem = CreateCompatibleDC(hdc);
				ERROR_CHECK_ZERO_EXTRA(hdcMem, L"CreateCompatibleDC", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY", goto pixelarray_DeleteObject);
				
				{
					HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
					ERROR_CHECK_NONZERO_EXTRA(!oldBitmap || oldBitmap == HGDI_ERROR, L"SelectObject", L"WndProc_paint_mandel/RENDER_MODES::BITMAP_PIXELARRAY", goto pixelarray_DeleteDC);
					
					// https://stackoverflow.com/questions/3688409/getdibits-and-loop-through-pixels-using-x-y
					
					// getting pixel header data
					
					BITMAPINFO hBitmapInfo = { 0 };
					hBitmapInfo.bmiHeader.biSize = sizeof(hBitmapInfo.bmiHeader);
					
					if (GetDIBits(hdcMem, hBitmap, 0, 0, NULL, &hBitmapInfo, DIB_RGB_COLORS) == 0) {
						MessageBox(NULL,
							L"Call to GetDIBits to load bitmap header data failed",
							L"HyperMandel",
							NULL);
					} else {
						std::unique_ptr<BYTE[]> lpPixels = std::make_unique<BYTE[]>(hBitmapInfo.bmiHeader.biSizeImage);
						hBitmapInfo.bmiHeader.biBitCount = 32;
						hBitmapInfo.bmiHeader.biCompression = BI_RGB;
						hBitmapInfo.bmiHeader.biHeight = abs(hBitmapInfo.bmiHeader.biHeight);
						
						bool setDIBitsSuccess = false;
						
						if (hBitmapInfo.bmiHeader.biSizeImage > 0) {
							// getting pixel data unnecessary because it will be fully reset anyway
							
							/*if (GetDIBits(hdcMem, hBitmap, 0, hBitmapInfo.bmiHeader.biHeight, lpPixels, &hBitmapInfo, DIB_RGB_COLORS) == 0) {
								MessageBox(NULL,
									L"Call to GetDIBits to load bitmap data failed",
									L"HyperMandel",
									NULL);
								break;
							}*/
							
							// setting pixels
							
							for (int y = 0; y < renderSize.height; y++) {
								for (int x = 0; x < renderSize.width; x++) {
									unsigned int basePixelIndex = x * 4 + y * renderSize.width * 4;
									
									if (basePixelIndex < hBitmapInfo.bmiHeader.biSizeImage) {
										// apparently basePixelIndex could go over somehow, not for colorRefArr like the others but lpPixels instead
										
										// capping colorRefIndex just in case
										unsigned int colorRefIndex = x + (renderSize.height - y - 1) * renderSize.width;
										
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
								MessageBox(NULL,
									L"Call to SetDIBits failed",
									L"HyperMandel",
									NULL);
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
			pixelarray_DeleteDC:
				DeleteDC(hdcMem);
			}
		pixelarray_DeleteObject:
			DeleteObject(hBitmap);
			break;
		}
		}
	}
}
