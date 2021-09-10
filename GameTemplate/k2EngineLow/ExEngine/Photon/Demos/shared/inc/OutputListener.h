#pragma once

#include <iostream>
#include "Common-cpp/inc/Common.h"
#if defined _EG_ANDROID_PLATFORM
#	include <android/log.h>
#elif defined _EG_SWITCH_WINDOWS_PLATFORM
#	include <nn/nn_Log.h>
#endif

class OutputListener : public virtual ExitGames::Common::BaseListener
{
public:
	virtual ~OutputListener(void){};
	virtual void write(const ExitGames::Common::JString& str) = 0;
	virtual void writeLine(const ExitGames::Common::JString& str=ExitGames::Common::JString()) = 0;
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& str)
	{
#	if defined _EG_MARMALADE_PLATFORM
		std::cerr << str.UTF8Representation().cstr() << std::endl;
#	elif defined _EG_SWITCH_WINDOWS_PLATFORM
		NN_LOG((str+L"\n").UTF8Representation().cstr());
#	else
		std::wcerr << str << std::endl;
#	endif
#	if defined _EG_MICROSOFT_PLATFORM
		OutputDebugStringW((str+L"\n").cstr());
#	endif
#	if defined _EG_ANDROID_PLATFORM
		int logLevel;
		switch(debugLevel)
		{
		case ExitGames::Common::DebugLevel::ALL:
			logLevel = ANDROID_LOG_DEBUG;
			break;
		case ExitGames::Common::DebugLevel::INFO:
			logLevel = ANDROID_LOG_INFO;
			break;
		case ExitGames::Common::DebugLevel::WARNINGS:
			logLevel = ANDROID_LOG_WARN;
			break;
		case ExitGames::Common::DebugLevel::ERRORS:
			logLevel = ANDROID_LOG_ERROR;
			break;
		case ExitGames::Common::DebugLevel::OFF:
			logLevel = ANDROID_LOG_FATAL;
			break;
		default:
			logLevel = ANDROID_LOG_FATAL;
			__android_log_write(logLevel, "Photon-Demo", "unknown log level");
			break;
		}
		__android_log_write(logLevel, "Photon-Demo", str.UTF8Representation().cstr());
#	endif
	}
};