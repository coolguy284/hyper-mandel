#pragma once

#include <cmath>
#include <numbers>

#include <Windows.h>

constexpr bool CARDIOID_SKIP = true;
constexpr bool PER2_BULB_SKIP = true;
constexpr int MAX_ITERS = 100;

namespace mandel {
	namespace calc {
		// converts cx, cy, zoom, and other values into input for multipixel
		
		template<typename T>
		struct Coords {
			// mathematical params
			
			T cx = 0.0;
			T cy = 0.0;
			
			T zoom = 0.25; // 1 / zoom is approximate diameter of render
			
			T rotation = 0.0;
			enum class ROTATION_UNIT {
				RADIANS,
				DEGREES,
				GRADIANS,
			} rotation_unit = ROTATION_UNIT::DEGREES;
			
			// technical params
			
			unsigned int width = 0; // initalized with a safe default but in reality should always be set
			unsigned int height = 0; // initalized with a safe default but in reality should always be set
			
			// relative location inside pixel to use, for x, 0.0 is left edge, 1.0 is right edge
			T subpixel_rel_x = 0.5;
			T subpixel_rel_y = 0.5;
			
			// enum for how to handle aspect ratios that aren't a square
			enum class ZOOM_MODE {
				Y_STABLE, // x axis will be zoomed in or out, y axis will not change zoom
				X_STABLE, // x axis will not change zoom, y axis will be zoomed in or out
				GROW, // when width > height, y zoom stays the same, when height > width, x zoom stays the same; therefore preference is to always zoom out
				SHRINK, // when width > height, x zoom stays the same, when height > width, y zoom stays the same; therefore preference is to always zoom in
			} zoom_mode = ZOOM_MODE::GROW;
		};
		
		template<typename T>
		struct Basic_MultiPixel_Args {
			T start_cx;
			T start_cy;
			
			// x and y screen coordinates each can step in both cx and cy directions (this allows rotation or any other linear transformation to the mandelbrot set, such as a skew)
			T x_step_cx;
			T x_step_cy;
			T y_step_cx;
			T y_step_cy;
			
			unsigned int x_count;
			unsigned int y_count;
		};
		
		// converts x, y, zoom values into input for multipixel

		template <typename T>
		Basic_MultiPixel_Args<T> convert_coord_to_multipixel(const Coords<T> coords);
		
		// returns the iteration count of a single C value in the mandelbrot set
		
		// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
		int basic_singlepixel(
			const float cx,
			const float cy
		);
		
		// fills iterCountArr with a 2d array of mandelbrot set values, following start, step, and count parameters
		// array order is x + y * width
		
		void basic_multipixel(
			const Basic_MultiPixel_Args<float> args,
			_Out_writes_all_(args.x_count * args.y_count) int* iterCountArr
		);
	}
}

#include "calculate.tpp"
