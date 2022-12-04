#define VERSION L"v0.1.0-alpha"

// until something useful is placed in the ui box, set to 0 instead of 200
#define UI_WIDTH 0

// magic number for height of title bar
#define TITLEBAR_HEIGHT 43

#define RENDER_MODE 2
#define RENDER_MODE_FILLRECT 0
#define RENDER_MODE_BITMAP_SETPIXEL 1
#define RENDER_MODE_BITMAP_PIXELARRAY 2

#include <Windows.h>
#include <sstream>

#include "calculate.h"
#include "render.h"

struct WIDHEIGHT {
	unsigned int width;
	unsigned int height;
};

static WCHAR szWindowClass[] = L"DesktopApp"; // window "class" (will find out later)
static WCHAR szTitle[] = L"HyperMandel " VERSION; // window title

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	// handles window events sent by windows os

	switch (message) {
	case WM_ERASEBKGND:
		// ignore this event because erasing background causes white flickering when resizing which is so bad it's seizure inducing
		return 1;

	case WM_PAINT: {
		// called when window needs to be updated (repainted)

		RECT winRect;

		if (!GetWindowRect(hWnd, &winRect)) {
			MessageBox(NULL,
				L"Call to GetWindowRect failed",
				L"HyperMandel",
				NULL);
			break;
		}

		// get size of paintable area inside window

		WIDHEIGHT windowSize;

		windowSize.width = winRect.right - winRect.left;
		windowSize.height = max(winRect.bottom - winRect.top - TITLEBAR_HEIGHT, 0); // title bar doesn't count

		std::wostringstream paintDebugOutputStream;

		paintDebugOutputStream << L"WM_PAINT: Window Size: ";
		paintDebugOutputStream << windowSize.width;
		paintDebugOutputStream << L", ";
		paintDebugOutputStream << windowSize.height;
		paintDebugOutputStream << L"\n";

		std::wstring paintDebugOutput = paintDebugOutputStream.str();

		OutputDebugString(paintDebugOutput.c_str());

		WIDHEIGHT renderSize;
		renderSize.width = max(windowSize.width - UI_WIDTH, 0); // UI_WIDTH less because removing right side for UI
		renderSize.height = windowSize.height;

		// painting

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// ignore everything but UI panel if window too small

		if (windowSize.width > UI_WIDTH) {
			unsigned int mandelArrayLength = renderSize.width * renderSize.height;

			if (mandelArrayLength > 0) {
				// calculate mandelbrot set and coloring

				int* iterCountArr = new int[mandelArrayLength];

				long minDimension = min(renderSize.width, renderSize.height);

				mandel::calc::basic_multipixel(
					-2.0f / minDimension * renderSize.width,
					2.0f / minDimension * renderSize.height,
					4.0f / minDimension,
					-4.0f / minDimension,
					renderSize.width,
					renderSize.height,
					iterCountArr
				);

				COLORREF* colorRefArr = new COLORREF[mandelArrayLength];

				mandel::render::convert_iterctarr_to_colorarr(iterCountArr, colorRefArr, mandelArrayLength);

				delete[] iterCountArr;

				// different render mode options

				switch (RENDER_MODE) {
				case RENDER_MODE_FILLRECT: {
					// calls fillrect on every pixel, ungodly slow

					RECT fr;
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

								FillRect(hdc, &fr, br);

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

					HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);

					HDC hdcMem = CreateCompatibleDC(hdc);
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

					BITMAP bitmap;
					GetObject(hBitmap, sizeof(bitmap), &bitmap);
					BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

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

					HBITMAP hBitmap = CreateCompatibleBitmap(hdc, renderSize.width, renderSize.height);

					HDC hdcMem = CreateCompatibleDC(hdc);
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
						break;
					}

					BYTE* lpPixels = new BYTE[hBitmapInfo.bmiHeader.biSizeImage];
					hBitmapInfo.bmiHeader.biBitCount = 32;
					hBitmapInfo.bmiHeader.biCompression = BI_RGB;
					hBitmapInfo.bmiHeader.biHeight = abs(hBitmapInfo.bmiHeader.biHeight);

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
									COLORREF color = colorRefArr[x + (renderSize.height - y - 1) * renderSize.width];
									lpPixels[basePixelIndex + 2] = GetRValue(color);
									lpPixels[basePixelIndex + 1] = GetGValue(color);
									lpPixels[basePixelIndex] = GetBValue(color);
								}
							}
						}

						// copying edited pixel data back to bitmap

						if (SetDIBits(hdcMem, hBitmap, 0, hBitmapInfo.bmiHeader.biHeight, lpPixels, &hBitmapInfo, DIB_RGB_COLORS) == 0) {
							MessageBox(NULL,
								L"Call to SetDIBits failed",
								L"HyperMandel",
								NULL);
							break;
						}
					}

					// free variables

					delete[] lpPixels;

					// rendering bitmap to screen

					BITMAP bitmap;
					GetObject(hBitmap, sizeof(bitmap), &bitmap);
					BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

					// cleaning up

					SelectObject(hdcMem, oldBitmap);
					DeleteDC(hdcMem);
					DeleteObject(hBitmap);
					break;
				}
				}

				// free variables

				delete[] colorRefArr;
			}
		}
		
		// paint ui on right (currently blank)

		RECT uiRect;
		uiRect.left = renderSize.width;
		uiRect.top = 0;
		uiRect.right = windowSize.width;
		uiRect.bottom = windowSize.height;

		FillRect(hdc, &uiRect, (HBRUSH)(COLOR_WINDOW + 1) /* white */);

		// paint finished

		EndPaint(hWnd, &ps);

		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow
) {
	// create window class

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1) /* white */;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	// register window class

	if (!RegisterClassEx(&wcex)) {
		// register failure

		MessageBox(NULL,
			L"Call to RegsiterClassEx failed",
			L"HyperMandel",
			NULL);

		return 1;
	}

	// create window
	
	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		854 + UI_WIDTH, 480 + TITLEBAR_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	// check if window created

	if (!hWnd) {
		MessageBox(NULL,
			L"Call to CreateWindowEx failed",
			L"HyperMandel",
			NULL);

		return 1;
	}

	// procure window

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// handle window messages

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
