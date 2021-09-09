#include "StdIO_UIListener.h"
#include <iostream>

int EG_kbhit();

using namespace ExitGames::Common;

#ifdef _EG_WINDOWS_PLATFORM
#include <windows.h>
#include <conio.h>

int EG_kbhit()
{
	int res = _kbhit();
	if(res)
		return _getch();
	return res;
}

#else
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>

int EG_kbhit()
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
		return ch;

	return 0;
}
#endif

void StdIO_UIListener::writeString(const JString& str)
{
	std::lock_guard<std::mutex> lock(mLastString);
	if(mLastString != str)
		std::wcout << (mLastString=str) << std::endl;
}

bool StdIO_UIListener::anyKeyPressed(void) const
{
	return !!EG_kbhit();
}

int StdIO_UIListener::getKeyPressed(void) const
{
	return EG_kbhit();
}

void StdIO_UIListener::onLibClosed(void)
{
	writeString(L"- -Press any key to quit- ------------------\n");
	while(!anyKeyPressed());
}