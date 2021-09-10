#include "Console.h"

Console::Console(void)
{
}

Console::~Console(void)
{
}

void Console::update(void)
{
}

void Console::write(const ExitGames::Common::JString& str)
{
#if defined _EG_MARMALADE_PLATFORM
	std::cout << str.UTF8Representation().cstr() << std::flush;
#else
	std::wcout << str << std::flush;
#endif
#if defined _EG_MICROSOFT_PLATFORM
	OutputDebugStringW(str.cstr());
#endif
}

void Console::writeLine(const ExitGames::Common::JString& str)
{
#if defined _EG_MARMALADE_PLATFORM
	std::cout << str.UTF8Representation().cstr() << std::endl;
#else
	std::wcout << str << std::endl;
#endif
#if defined _EG_MICROSOFT_PLATFORM
	OutputDebugStringW((str+L"\n").cstr());
#endif
}

Console& Console::get(void)
{
	static Console console;
	return console;
}