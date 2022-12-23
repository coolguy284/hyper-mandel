#pragma once

#include "error_msg_box.h"

// wraps Win32 API call in a MsgBox indicating it errored.
#define ERROR_WRAP(funcCall, funcName, desc) {\
	BOOL _result = funcCall;\
	if (!_result) {\
		WCHAR _funcName[] = funcName;\
		WCHAR _desc[] = desc;\
		errorMsgBox(_funcName, _desc);\
	}\
}

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
