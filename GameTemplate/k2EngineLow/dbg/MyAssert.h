/*!
 *@brief	アサート
 */

#pragma once

#include <stdarg.h>

namespace nsK2EngineLow {
	/*!
		*@brief	アサート
		*@param[in]	flag	偽のときアサートが発生します。
		*/
	static inline void MyAssert(bool flag, const char* format, const char* file, long line, ...)
	{
		if (!flag) {
			va_list va;
			va_start(va, flag);
			vprintf(format, va);
			char fileLineInfo[256];
			sprintf_s(fileLineInfo, "%s, %d行目", file, line);
			char assertMessage[256];
			vsprintf_s(assertMessage, format, va);
			strcat_s(assertMessage, fileLineInfo);
			MessageBoxA(nullptr, assertMessage, "アサート", MB_OK);
			va_end(va);
			std::abort();
		}
	}
}

#ifdef K2_DEBUG
#define 	K2_ASSERT( flg, format, ... )	nsK2EngineLow::MyAssert( flg, format, __FILE__, __LINE__, __VA_ARGS__)
#else
#define 	K2_ASSERT( flg, format, ... )
#endif
