/*!
 *@brief		ÉçÉO
 */

#pragma once

namespace nsK2EngineLow {
	/*!
	 *@brief	ÉçÉOèoóÕ
	 */
	static inline void Log( const char* format, ... )
	{
		static char log[1024*10];
		va_list va;
		va_start( va, format );
		vsprintf( log, format, va );
		OutputDebugStringA( log );
		va_end( va );
	}
}

#ifdef _DEBUG
	#define MY_LOG( format, ... )	Log(format, __VA_ARGS__)
#else // _DEBUG
#define MY_LOG( format, ... )
#endif // _DEBUG
