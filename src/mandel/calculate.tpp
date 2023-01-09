#pragma once

#include "calculate.hpp"

namespace mandel {
	namespace calc {
		// converts x, y, zoom values into input for multipixel
		
		template <typename T>
		Basic_MultiPixel_Args<T> convert_coord_to_multipixel(const Coords<T> coords) {
			Basic_MultiPixel_Args<T> mpArgs = { 0 };
			
			// calculate non rotated view
			
			T nonRotated_x_step_cx = 0.0;
			T nonRotated_y_step_cy = 0.0;
			
			switch (coords.zoom_mode) {
			case Coords<T>::ZOOM_MODE::Y_STABLE:
				nonRotated_y_step_cy = -1.0 / coords.zoom / coords.height;
				nonRotated_x_step_cx = -nonRotated_y_step_cy;
				break;
			
			case Coords<T>::ZOOM_MODE::X_STABLE:
				nonRotated_x_step_cx = 1.0 / coords.zoom / coords.width;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			
			case Coords<T>::ZOOM_MODE::GROW: {
				int minDimension = min(coords.width, coords.height);
				
				nonRotated_x_step_cx = 1.0 / coords.zoom / minDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			
			case Coords<T>::ZOOM_MODE::SHRINK: {
				int maxDimension = max(coords.width, coords.height);
				
				nonRotated_x_step_cx = 1.0 / coords.zoom / maxDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			}
			
			T nonRotated_start_cx_delta = -(nonRotated_x_step_cx * coords.width / 2.0) + coords.subpixel_rel_x * nonRotated_x_step_cx;
			T nonRotated_start_cy_delta = -(nonRotated_y_step_cy * coords.height / 2.0) + coords.subpixel_rel_y * nonRotated_y_step_cy;
			
			// rotate variables by given rotation
			
			T rotationRads = 0.0;
			switch (coords.rotation_unit) {
			case Coords<T>::ROTATION_UNIT::RADIANS:
				rotationRads = coords.rotation;
				break;
			
			case Coords<T>::ROTATION_UNIT::DEGREES:
				rotationRads = coords.rotation / 180.0 * std::numbers::pi_v<T>;
				break;
			
			case Coords<T>::ROTATION_UNIT::GRADIANS:
				rotationRads = coords.rotation / 200.0 * std::numbers::pi_v<T>;
				break;
			}
			
			mpArgs.start_cx = (cosf(rotationRads) * nonRotated_start_cx_delta - sinf(rotationRads) * nonRotated_start_cy_delta) + coords.cx;
			mpArgs.start_cy = (cosf(rotationRads) * nonRotated_start_cy_delta + sinf(rotationRads) * nonRotated_start_cx_delta) + coords.cy;
			mpArgs.x_step_cx = cosf(rotationRads) * nonRotated_x_step_cx;
			mpArgs.x_step_cy = -sinf(rotationRads) * nonRotated_y_step_cy;
			mpArgs.y_step_cx = sinf(rotationRads) * nonRotated_x_step_cx;
			mpArgs.y_step_cy = cosf(rotationRads) * nonRotated_y_step_cy;
			
			// add in width and height
			
			mpArgs.x_count = coords.width;
			mpArgs.y_count = coords.height;
			
			return mpArgs;
		}
		
		// returns the iteration count of a single C value in the mandelbrot set
		
		template <typename T>
		int basic_singlepixel(
			const T cx,
			const T cy
		) {
			// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
			if (CARDIOID_SKIP) {
				T qt1 = cx - 0.25f;
				T q = qt1 * qt1 + cy * cy;
				if (q * (q + (cx - 0.25f)) < 0.25f * cy * cy) {
					// in cardioid
					return MAX_ITERS;
				}
			}
			
			if (PER2_BULB_SKIP) {
				T t1 = cx + 1.0f;
				if (t1 * t1 + cy * cy < (1.0f / 16.0f)) {
					// in period-2 bulb
					return MAX_ITERS;
				}
			}
			
			T zx = 0.0;
			T zy = 0.0;
			
			int iterCount = 0;
			
			while (zx * zx + zy * zy < 4.0f && iterCount < MAX_ITERS) {
				// z = z^2 + c
				// (a + bi)^2 = (a*a + 2*a*b*i + b*b*-1) = (a*a - b*b) + (2*a*b)*i
				
				T z2x = zx * zx - zy * zy;
				T z2y = 2 * zx * zy;
				
				zx = z2x + cx;
				zy = z2y + cy;
				
				iterCount++;
			}
			
			return iterCount;
		}
	}
}
