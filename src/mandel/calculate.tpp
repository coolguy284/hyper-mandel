#pragma once

#include "calculate.hpp"

namespace mandel {
	namespace calc {
		// converts x, y, zoom values into input for multipixel
		
		template <typename T>
		Basic_MultiPixel_Args<T> convert_coord_to_multipixel(const Coords<T> coords) {
			Basic_MultiPixel_Args<T> mpArgs = { 0 };
			
			// calculate non rotated view
			
			T nonRotated_x_step_cx = 0.0L;
			T nonRotated_y_step_cy = 0.0L;
			
			switch (coords.zoom_mode) {
			case Coords<T>::ZOOM_MODE::Y_STABLE:
				nonRotated_y_step_cy = (T)-1.0L / coords.zoom / coords.height;
				nonRotated_x_step_cx = -nonRotated_y_step_cy;
				break;
			
			case Coords<T>::ZOOM_MODE::X_STABLE:
				nonRotated_x_step_cx = (T)1.0L / coords.zoom / coords.width;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			
			case Coords<T>::ZOOM_MODE::GROW: {
				int minDimension = min(coords.width, coords.height);
				
				nonRotated_x_step_cx = (T)1.0L / coords.zoom / minDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			
			case Coords<T>::ZOOM_MODE::SHRINK: {
				int maxDimension = max(coords.width, coords.height);
				
				nonRotated_x_step_cx = (T)1.0L / coords.zoom / maxDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			}
			
			T nonRotated_start_cx_delta = -(nonRotated_x_step_cx * coords.width / (T)2.0L) + coords.subpixel_rel_x * nonRotated_x_step_cx;
			T nonRotated_start_cy_delta = -(nonRotated_y_step_cy * coords.height / (T)2.0L) + coords.subpixel_rel_y * nonRotated_y_step_cy;
			
			// rotate variables by given rotation
			
			T rotationRads = 0.0L;
			switch (coords.rotation_unit) {
			case Coords<T>::ROTATION_UNIT::RADIANS:
				rotationRads = coords.rotation;
				break;
			
			case Coords<T>::ROTATION_UNIT::DEGREES:
				rotationRads = coords.rotation / (T)180.0L * std::numbers::pi_v<T>;
				break;
			
			case Coords<T>::ROTATION_UNIT::GRADIANS:
				rotationRads = coords.rotation / (T)200.0L * std::numbers::pi_v<T>;
				break;
			}
			
			mpArgs.start_cx = (cos(rotationRads) * nonRotated_start_cx_delta - sin(rotationRads) * nonRotated_start_cy_delta) + coords.cx;
			mpArgs.start_cy = (cos(rotationRads) * nonRotated_start_cy_delta + sin(rotationRads) * nonRotated_start_cx_delta) + coords.cy;
			mpArgs.x_step_cx = cos(rotationRads) * nonRotated_x_step_cx;
			mpArgs.x_step_cy = -sin(rotationRads) * nonRotated_y_step_cy;
			mpArgs.y_step_cx = sin(rotationRads) * nonRotated_x_step_cx;
			mpArgs.y_step_cy = cos(rotationRads) * nonRotated_y_step_cy;
			
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
				T qt1 = cx - (T)0.25L;
				T q = qt1 * qt1 + cy * cy;
				if (q * (q + (cx - (T)0.25L)) < (T)0.25L * cy * cy) {
					// in cardioid
					return MAX_ITERS;
				}
			}
			
			if (PER2_BULB_SKIP) {
				T t1 = cx + (T)1.0L;
				if (t1 * t1 + cy * cy < (T)(1.0L / 16.0L)) {
					// in period-2 bulb
					return MAX_ITERS;
				}
			}
			
			T zx = 0.0L;
			T zy = 0.0L;
			
			int iterCount = 0;
			
			while (zx * zx + zy * zy < (T)4.0L && iterCount < MAX_ITERS) {
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

		// fills iterCountArr with a 2d array of mandelbrot set values, following start, step, and count parameters
		// array order is x + y * width

		template <typename T>
		void basic_multipixel(
			const Basic_MultiPixel_Args<T> args,
			_Out_writes_all_(args.x_count* args.y_count) int* iterCountArr
		) {
			size_t index = 0;

			for (size_t y = 0; y < args.y_count; y++) {
				T y_cx = args.start_cx + args.y_step_cx * y;
				T y_cy = args.start_cy + args.y_step_cy * y;

				for (size_t x = 0; x < args.x_count; x++) {
					T cx = y_cx + args.x_step_cx * x;
					T cy = y_cy + args.x_step_cy * x;

					iterCountArr[index] = basic_singlepixel(cx, cy);

					index++;
				}
			}
		}
	}
}
