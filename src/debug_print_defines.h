#pragma once

#include <sstream>

#define PRINT_DEBUG_NOARG(msg) OutputDebugString(msg L"\n")

#define PRINT_DEBUG_2ARG(label, arg1, arg2) {\
	std::wostringstream debugOutputStream;\
	\
	debugOutputStream << label;\
	debugOutputStream << arg1;\
	debugOutputStream << L", ";\
	debugOutputStream << arg2;\
	debugOutputStream << L"\n";\
	\
	std::wstring debugOutput = debugOutputStream.str();\
	\
	OutputDebugString(debugOutput.c_str());\
}

#define PRINT_DEBUG_4ARG(label, arg1, arg2, arg3, arg4) {\
	std::wostringstream debugOutputStream;\
	\
	debugOutputStream << label;\
	debugOutputStream << arg1;\
	debugOutputStream << L", ";\
	debugOutputStream << arg2;\
	debugOutputStream << L", ";\
	debugOutputStream << arg3;\
	debugOutputStream << L", ";\
	debugOutputStream << arg4;\
	debugOutputStream << L"\n";\
	\
	std::wstring debugOutput = debugOutputStream.str();\
	\
	OutputDebugString(debugOutput.c_str());\
}
