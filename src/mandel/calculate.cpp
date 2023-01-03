#include "calculate.h"

namespace mandel {
	namespace calc {
		// converts x, y, zoom values into input for multipixel
		
		Basic_MultiPixel_Args convert_coord_to_multipixel(const Coords coords) {
			Basic_MultiPixel_Args mpArgs = { 0 };
			
			// calculate non rotated view
			
			float nonRotated_x_step_cx = 0.0f;
			float nonRotated_y_step_cy = 0.0f;
			
			switch (coords.zoom_mode) {
			case Coords::ZOOM_MODE::Y_STABLE:
				nonRotated_y_step_cy = -1.0f / coords.zoom / coords.height;
				nonRotated_x_step_cx = -nonRotated_y_step_cy;
				break;
			
			case Coords::ZOOM_MODE::X_STABLE:
				nonRotated_x_step_cx = 1.0f / coords.zoom / coords.width;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			
			case Coords::ZOOM_MODE::GROW: {
				int minDimension = min(coords.width, coords.height);
				
				nonRotated_x_step_cx = 1.0f / coords.zoom / minDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			
			case Coords::ZOOM_MODE::SHRINK: {
				int maxDimension = max(coords.width, coords.height);
				
				nonRotated_x_step_cx = 1.0f / coords.zoom / maxDimension;
				nonRotated_y_step_cy = -nonRotated_x_step_cx;
				break;
			}
			}
			
			float nonRotated_start_cx_delta = -(nonRotated_x_step_cx * coords.width / 2.0f) + coords.subpixel_rel_x * nonRotated_x_step_cx;
			float nonRotated_start_cy_delta = -(nonRotated_y_step_cy * coords.height / 2.0f) + coords.subpixel_rel_y * nonRotated_y_step_cy;
			
			// rotate variables by given rotation
			
			float rotationRads = 0.0f;
			switch (coords.rotation_unit) {
			case Coords::ROTATION_UNIT::RADIANS:
				rotationRads = coords.rotation;
				break;
			
			case Coords::ROTATION_UNIT::DEGREES:
				rotationRads = coords.rotation / 180.0f * std::numbers::pi_v<float>;
				break;
			
			case Coords::ROTATION_UNIT::GRADIANS:
				rotationRads = coords.rotation / 200.0f * std::numbers::pi_v<float>;
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
		
		int basic_singlepixel(
			const float cx,
			const float cy
		) {
			// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set
			if (CARDIOID_SKIP) {
				float qt1 = cx - 0.25f;
				float q = qt1 * qt1 + cy * cy;
				if (q * (q + (cx - 0.25f)) < 0.25f * cy * cy) {
					// in cardioid
					return MAX_ITERS;
				}
			}
			
			if (PER2_BULB_SKIP) {
				float t1 = cx + 1.0f;
				if (t1 * t1 + cy * cy < (1.0f / 16.0f)) {
					// in period-2 bulb
					return MAX_ITERS;
				}
			}
			
			float zx = 0.0;
			float zy = 0.0;
			
			int iterCount = 0;
			
			while (zx * zx + zy * zy < 4.0f && iterCount < MAX_ITERS) {
				// z = z^2 + c
				
				// (a + bi)^2 = (a*a + 2*a*b*i + b*b*-1) = (a*a - b*b) + (2*a*b)*i
				float z2x = zx * zx - zy * zy;
				float z2y = 2 * zx * zy;
				
				zx = z2x + cx;
				zy = z2y + cy;
				
				iterCount++;
			}
			
			return iterCount;
		}
		
		// fills iterCountArr with a 2d array of mandelbrot set values, following start, step, and count parameters
		// array order is x + y * width
		
		void basic_multipixel(
			const Basic_MultiPixel_Args args,
			_Out_writes_all_(args.x_count * args.y_count) int* iterCountArr
		) {
			size_t index = 0;
			
			for (size_t y = 0; y < args.y_count; y++) {
				float y_cx = args.start_cx + args.y_step_cx * y;
				float y_cy = args.start_cy + args.y_step_cy * y;
				
				for (size_t x = 0; x < args.x_count; x++) {
					float cx = y_cx + args.x_step_cx * x;
					float cy = y_cy + args.x_step_cy * x;
					
					iterCountArr[index] = basic_singlepixel(cx, cy);
					
					index++;
				}
			}
		}
	}
}
