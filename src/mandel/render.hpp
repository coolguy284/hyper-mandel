#pragma once

#include <Windows.h>

#include "calculate.h"

namespace mandel {
	namespace render {
		// gets the color of an iteration count using a rainbow pallete
		
		COLORREF get_iterct_color(int iterCount);
		
		// converts an iteration count array with a given length to a color array
		
		void convert_iterctarr_to_colorarr(
			_In_reads_(length) int* iterCountArr,
			_Out_writes_all_(length) COLORREF* colorRefArr,
			int length
		);
	}
}
