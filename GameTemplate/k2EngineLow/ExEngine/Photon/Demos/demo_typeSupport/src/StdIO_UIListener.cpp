#include "StdIO_UIListener.h"
#include <iostream>

int EG_kbhit(void);

#ifdef _EG_WINDOWS_PLATFORM
#include <conio.h>

int EG_kbhit(void)
{
	int res = _kbhit();
	if(res)
		_getch();
	return res;
}

#else
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>

int EG_kbhit(void)
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
		return 1;

	return 0;
}
#endif

void StdIO_UIListener::writeString(const ExitGames::Common::JString& str)
{
	std::wcout << str;
}

bool StdIO_UIListener::anyKeyPressed(void) const
{
	return (EG_kbhit() != 0);
}

void StdIO_UIListener::onLibClosed(void)
{
	writeString(L"- -Press any key to quit- ------------------\n");
	while(!anyKeyPressed());
}