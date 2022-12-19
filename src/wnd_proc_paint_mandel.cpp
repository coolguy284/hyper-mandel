#include "wnd_proc_paint_mandel.h"

void WndProc_paint_mandel(WIDHEIGHT renderSize, HDC* hdc) {
	unsigned int mandelArrayLength = renderSize.width * renderSize.height;
	
	if (mandelArrayLength > 0) {
		// calculate mandelbrot set and coloring
		
		std::unique_ptr<int[]> iterCountArr = std::make_unique<int[]>(mandelArrayLength);
		
		long minDimension = min(renderSize.width, renderSize.height);
		
		mandel::calc::basic_multipixel(
			-2.0f / minDimension * renderSize.width,
			2.0f / minDimension * renderSize.height,
			4.0f / minDimension,
			-4.0f / minDimension,
			renderSize.width,
			renderSize.height,
			iterCountArr.get()
		);
		
		std::unique_ptr<COLORREF[]> colorRefArr = std::make_unique<COLORREF[]>(mandelArrayLength);
		
		mandel::render::convert_iterctarr_to_colorarr(iterCountArr.get(), colorRefArr.get(), mandelArrayLength);
		
		// different render mode options
		
		switch (RENDER_MODE) {
		case RENDER_MODE_FILLRECT: {
			// calls fillrect on every pixel, ungodly slow
			
			RECT fr = { 0 };
			HBRUSH br;
			
			for (unsigned int y = 0; y < renderSize.height; y++) {
				for (unsigned int x = 0; x < renderSize.width; x++) {
					unsigned int baseColorIndex = x + y * renderSize.width;
					
					if (baseColorIndex < mandelArrayLength) {
						// apparently baseColorIndex could go over somehow
						fr.left = x;
						fr.top = y;
						fr.right = x + 1;
						fr.bottom = y + 1;
						
						br = CreateSolidBrush(colorRefArr[baseColorIndex]);
						
						FillRect(*hdc, &fr, br);
						
						DeleteObject(br);
					}
				}
			}
			break;
		}
		
		case RENDER_MODE_BITMAP_SETPIXEL: {
			// creates a bitmap, then calls SetPixel, then renders it, slow
			
			// https://stackoverflow.com/questions/1748470/how-to-draw-image-on-a-window
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(*hdc, renderSize.width, renderSize.height);
			
			HDC hdcMem = CreateCompatibleDC(*hdc);
			HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
			
			// setting pixels
			
			for (unsigned int y = 0; y < renderSize.height; y++) {
				for (unsigned int x = 0; x < renderSize.width; x++) {
					unsigned int baseColorIndex = x + y * renderSize.width;
					
					if (baseColorIndex < mandelArrayLength) {
						// apparently baseColorIndex could go over somehow
						SetPixel(hdcMem, x, y, colorRefArr[baseColorIndex]);
					}
				}
			}
			
			// rendering bitmap to screen
			
			BITMAP bitmap = { 0 };
			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(*hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
			
			// cleaning up
			
			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);
			DeleteObject(hBitmap);
			break;
		}
		
		case RENDER_MODE_BITMAP_PIXELARRAY: {
			// creates a bitmap, then locks it and edits pixel memory, then renders it, decent,
			// but still slower than calculating the mandelbrot set itself -.-
			
			// creating bitmap and handle to change pixels
			
			HBITMAP hBitmap = CreateCompatibleBitmap(*hdc, renderSize.width, renderSize.height);
			
			HDC hdcMem = CreateCompatibleDC(*hdc);
			HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
			
			// https://stackoverflow.com/questions/3688409/getdibits-and-loop-through-pixels-using-x-y
			
			// getting pixel data
			
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
					
					for (unsigned int y = 0; y < renderSize.height; y++) {
						for (unsigned int x = 0; x < renderSize.width; x++) {
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
					BitBlt(*hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
				}
			}
			
			// cleaning up
			
			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);
			DeleteObject(hBitmap);
			break;
		}
		}
	}
}
