#include "calculate.hpp"

namespace mandel {
	namespace calc {
		// fills iterCountArr with a 2d array of mandelbrot set values, following start, step, and count parameters
		// array order is x + y * width
		
		void basic_multipixel(
			const Basic_MultiPixel_Args<float> args,
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
