#pragma once

#include <Windows.h>

constexpr bool CARDIOID_SKIP = true;
constexpr bool PER2_BULB_SKIP = true;
constexpr int MAX_ITERS = 100;

namespace mandel {
	namespace calc {
		// converts cx, cy, zoom, and other values into input for multipixel
		
		struct Coords {
			float cx = 0.0f;
			float cy = 0.0f;
			float zoom = 0.25f; // 1 / zoom is approximate diameter of render
			
			unsigned int width = 0; // initalized with a safe default but in reality should always be set
			unsigned int height = 0; // initalized with a safe default but in reality should always be set
			
			// relative location inside pixel to use, for x, 0.0 is left edge, 1.0 is right edge
			float subpixel_rel_x = 0.5f;
			float subpixel_rel_y = 0.5f;
			
			// enum for how to handle aspect ratios that aren't a square
			enum class ZOOM_MODE {
				Y_STABLE, // x axis will be zoomed in or out, y axis will not change zoom
				X_STABLE, // x axis will not change zoom, y axis will be zoomed in or out
				GROW, // when width > height, y zoom stays the same, when height > width, x zoom stays the same; therefore preference is to always zoom out
				SHRINK, // when width > height, x zoom stays the same, when height > width, y zoom stays the same; therefore preference is to always zoom in
			} zoom_mode = ZOOM_MODE::GROW;
		};
		
		struct Basic_MultiPixel_Args {
			float cxStart;
			float cyStart;
			
			float cxStep;
			float cyStep;
			
			unsigned int cxCount;
			unsigned int cyCount;
		};
		
		Basic_MultiPixel_Args convert_coord_to_multipixel(const Coords coords);
		
		// returns the iteration count of a single C value in the mandelbrot set
		
		// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
		int basic_singlepixel(
			const float cx,
			const float cy
		);
		
		// fills iterCountArr with a 2d array of mandelbrot set values, following start, step, and count parameters
		// array order is x + y * width
		
		void basic_multipixel(
			const Basic_MultiPixel_Args args,
			_Out_writes_all_(args.cxCount * args.cyCount) int* iterCountArr
		);
	}
}
