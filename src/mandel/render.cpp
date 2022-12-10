#include "render.h"

namespace mandel {
	namespace render {
		// gets the color of an iteration count using a rainbow pallete

		COLORREF get_iterct_color(int iterCount) {
			if (iterCount < 0 || iterCount >= MAX_ITERS) return RGB(0, 0, 0);

			int iterCountMod = (-(iterCount - 1) % 24 + 24) % 24; // minus sign at front because backwards rainbow looks better

			switch (iterCountMod) {
			case 0: return RGB(255, 0, 0);
			case 1: return RGB(255, 63, 0);
			case 2: return RGB(255, 127, 0);
			case 3: return RGB(255, 191, 0);
			case 4: return RGB(255, 255, 0);
			case 5: return RGB(191, 255, 0);
			case 6: return RGB(127, 255, 0);
			case 7: return RGB(63, 255, 0);
			case 8: return RGB(0, 255, 0);
			case 9: return RGB(0, 255, 63);
			case 10: return RGB(0, 255, 127);
			case 11: return RGB(0, 255, 191);
			case 12: return RGB(0, 255, 255);
			case 13: return RGB(0, 191, 255);
			case 14: return RGB(0, 127, 255);
			case 15: return RGB(0, 63, 255);
			case 16: return RGB(0, 0, 255);
			case 17: return RGB(63, 0, 255);
			case 18: return RGB(127, 0, 255);
			case 19: return RGB(191, 0, 255);
			case 20: return RGB(255, 0, 255);
			case 21: return RGB(255, 0, 191);
			case 22: return RGB(255, 0, 127);
			case 23: return RGB(255, 0, 63);
			default: return RGB(0, 0, 0); // should be impossible branch but here to remove the warning
			}
		}

		// converts an iteration count array with a given length to a color array

		void convert_iterctarr_to_colorarr(
			_In_reads_(length) int* iterCountArr,
			_Out_writes_all_(length) COLORREF* colorRefArr,
			int length
		) {
			for (int i = 0; i < length; i++) {
				colorRefArr[i] = get_iterct_color(iterCountArr[i]);
			}
		}
	}
}
