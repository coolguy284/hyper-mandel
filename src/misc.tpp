#pragma once

#include "misc.hpp"

// https://stackoverflow.com/questions/13794171/how-to-make-the-mod-of-a-negative-number-to-be-positive/50900650#50900650
template <typename T>
T mod_pos(T num, T divisor) {
	T result = fmod(num, divisor);
	return result + (result < 0 ? divisor : 0);
}

template <typename T>
std::wstring real_number_to_string(T val) {
	std::wostringstream strVal;

	strVal << std::setprecision(std::numeric_limits<T>::max_digits10);

	strVal << val;

	return strVal.str();
}
