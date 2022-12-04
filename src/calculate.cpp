#include "calculate.h"

// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set

namespace mandel {
	namespace calc {
		// returns the iteration count of a single C value in the mandelbrot set

		int basic_singlepixel(
			float cx,
			float cy
		) {
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
			float cxStart,
			float cyStart,
			float cxStep,
			float cyStep,
			int cxCount,
			int cyCount,
			_Out_writes_all_(cxCount* cyCount) int* iterCountArr
		) {
			int index = 0;

			float cyCurrent = cyStart;
			for (int y = 0; y < cyCount; y++) {
				float cxCurrent = cxStart;

				for (int x = 0; x < cxCount; x++) {
					iterCountArr[index] = basic_singlepixel(cxCurrent, cyCurrent);

					index++;
					cxCurrent += cxStep;
				}

				cyCurrent += cyStep;
			}
		}
	}
}
