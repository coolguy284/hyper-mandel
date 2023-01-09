#pragma once

// https://stackoverflow.com/questions/13794171/how-to-make-the-mod-of-a-negative-number-to-be-positive/50900650#50900650
template<class T>
T mod_pos(T num, T divisor) {
	T result = fmod(num, divisor);
	return result + (result < 0 ? divisor : 0);
}
