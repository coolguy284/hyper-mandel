#pragma once

#include <Windows.h>

// https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set

constexpr bool CARDIOID_SKIP = true;
constexpr bool PER2_BULB_SKIP = true;
constexpr int MAX_ITERS = 100;

namespace mandel {
	namespace calc {
		// returns the iteration count of a single C value in the mandelbrot set
		
		int basic_singlepixel(
			float cx,
			float cy
		);
		
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
		);
	}
}
