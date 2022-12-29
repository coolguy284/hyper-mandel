#include "calculate.h"

namespace mandel {
	namespace calc {
		// converts x, y, zoom values into input for multipixel
		
		Basic_MultiPixel_Args convert_coord_to_multipixel(const Coords coords) {
			Basic_MultiPixel_Args mpArgs = { 0 };
			
			switch (coords.zoom_mode) {
			case Coords::ZOOM_MODE::Y_STABLE:
				mpArgs.cyStep = -1.0f / coords.zoom / coords.height;
				mpArgs.cxStep = -mpArgs.cyStep;
				break;
			
			case Coords::ZOOM_MODE::X_STABLE:
				mpArgs.cxStep = 1.0f / coords.zoom / coords.width;
				mpArgs.cyStep = -mpArgs.cxStep;
				break;
			
			case Coords::ZOOM_MODE::GROW: {
				int minDimension = min(coords.width, coords.height);
				
				mpArgs.cxStep = 1.0f / coords.zoom / minDimension;
				mpArgs.cyStep = -mpArgs.cxStep;
				break;
			}
			
			case Coords::ZOOM_MODE::SHRINK: {
				int maxDimension = max(coords.width, coords.height);
				
				mpArgs.cxStep = 1.0f / coords.zoom / maxDimension;
				mpArgs.cyStep = -mpArgs.cxStep;
				break;
			}
			}
			
			mpArgs.cxStart = coords.cx - (mpArgs.cxStep * coords.width / 2.0f) + coords.subpixel_rel_x * mpArgs.cxStep;
			mpArgs.cyStart = coords.cy - (mpArgs.cyStep * coords.height / 2.0f) + coords.subpixel_rel_y * mpArgs.cyStep;
			
			mpArgs.cxCount = coords.width;
			mpArgs.cyCount = coords.height;
			
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
			_Out_writes_all_(args.cxCount * args.cyCount) int* iterCountArr
		) {
			size_t index = 0;
			
			float cyCurrent = args.cyStart;
			for (size_t y = 0; y < args.cyCount; y++) {
				float cxCurrent = args.cxStart;
				
				for (size_t x = 0; x < args.cxCount; x++) {
					iterCountArr[index] = basic_singlepixel(cxCurrent, cyCurrent);
					
					index++;
					cxCurrent += args.cxStep;
				}
				
				cyCurrent += args.cyStep;
			}
		}
	}
}
