#include "Console.h"
#include "NetworkLogic.h"

#if defined _EG_EMSCRIPTEN_PLATFORM
#	include <emscripten.h>
#endif

int getcharIfKbhit(void);

#ifdef _EG_WINDOWS_PLATFORM
#include <conio.h>

int getcharIfKbhit(void)
{
	int res = _kbhit();
	if(res)
		res = _getch();
	return res;
}
#elif defined _EG_PS4_PLATFORM
#include "Controller.h"

int getcharIfKbhit(void)
{
	static Controller controller;
	return controller.readInput();
}
#elif defined _EG_EMSCRIPTEN_PLATFORM
// no console input supported in browser app currently
int getcharIfKbhit(void)
{
	return 100;
}
#elif defined _EG_PSVITA_PLATFORM || defined _EG_XB1_PLATFORM || defined _EG_SWITCH_PLATFORM || defined _EG_GAMECORE_PLATFORM || defined _EG_PS5_PLATFORM
int getcharIfKbhit(void)
{
	return 0; // no input handling implemented
}
#else
#include <fcntl.h>
#include <termios.h>

int getcharIfKbhit(void)
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

	return ch;
}
#endif

#if defined _EG_EMSCRIPTEN_PLATFORM
#	undef SLEEP
#	define SLEEP(ms) // update rate set in emscripten_set_main_loop call
#endif


class StdUI: public NetworkLogicListener
{
public:
	StdUI(void);
	void main(void);
	void main_loop(void);

	static NetworkLogic& getNetworkLogic(void);
private:
	virtual void stateUpdate(State newState){}
	void usage(void);

	bool mExit;
};

StdUI::StdUI(void) 
	: mExit(false)
{}

void StdUI::usage(void)
{
#if defined _EG_SONY_PLATFORM
	Console::get().writeLine(L"Photon LoadBalancing Demo");
	Console::get().writeLine(L"usage:");
	Console::get().writeLine(L"always:");
	Console::get().writeLine(L" Triangle - print this help");
	Console::get().writeLine(L" Circle - exit");
	Console::get().writeLine(L"--------------------");
	Console::get().writeLine(L"outside a game room:");
	Console::get().writeLine(L" Cross - create game");
	Console::get().writeLine(L" Square - join random game or last joined game");
	Console::get().writeLine(L" L2 - create direct \"all to all\" game");
	Console::get().writeLine(L" R2 - create direct \"master to all\" game");
	Console::get().writeLine(L"--------------------");
	Console::get().writeLine(L"inside a game room:");
	Console::get().writeLine(L" Cross - leave game");
	Console::get().writeLine(L" Square - leave game (will come back - disconnects and quits)");
	Console::get().writeLine(L"--------------------");
#else
	Console::get().writeLine(L"Photon LoadBalancing Demo");
	Console::get().writeLine(L"usage:");
	Console::get().writeLine(L"always:");
	Console::get().writeLine(L" h - print this help");
	Console::get().writeLine(L" 0 - exit");
	Console::get().writeLine(L"--------------------");
	Console::get().writeLine(L"outside a game room:");
	Console::get().writeLine(L" 1 - create game");
	Console::get().writeLine(L" 2 - join random game or last joined game");
	Console::get().writeLine(L" 3 - create direct \"all to all\" game");
	Console::get().writeLine(L" 4 - create direct \"master to all\" game");
	Console::get().writeLine(L"--------------------");
	Console::get().writeLine(L"inside a game room:");
	Console::get().writeLine(L" 1 - leave game");
	Console::get().writeLine(L" 2 - leave game (will come back - disconnects and quits)");
	Console::get().writeLine(L"--------------------");
#endif
}

static StdUI ui;

#ifdef _EG_EMSCRIPTEN_PLATFORM
void main_loop()
{
	ui.main_loop();
}
#endif

void StdUI::main(void)
{
	usage();
	getNetworkLogic().connect();
#ifdef _EG_EMSCRIPTEN_PLATFORM
	emscripten_set_main_loop(::main_loop, 30, 0);
#else
	while(getNetworkLogic().getLastInput() != INPUT_EXIT && !mExit)
		main_loop();
#endif
}

void StdUI::main_loop(void)
{
	int ch = getcharIfKbhit();
	switch(ch)
	{
		case 'h':
			usage();
			break;
		case '0':
			getNetworkLogic().setLastInput(INPUT_EXIT);
			mExit = true;
#if defined _EG_EMSCRIPTEN_PLATFORM
			emscripten_force_exit(0);
#endif
			break;
		default:
			getNetworkLogic().setLastInput(INPUT_NON);
			break;
	}
	switch(ch)
	{
	case '1':
		getNetworkLogic().setLastInput(INPUT_1);
		break;
	case '2':
		getNetworkLogic().setLastInput(INPUT_2);
		break;
	case '3':
		getNetworkLogic().setLastInput(INPUT_3);
		break;
	case '4':
		getNetworkLogic().setLastInput(INPUT_4);
		break;
	}
	getNetworkLogic().run();
	Console::get().update();
	SLEEP(100);
}

NetworkLogic& StdUI::getNetworkLogic(void)
{
	static NetworkLogic networkLogic(&Console::get());
	return networkLogic;
}

#if !defined _EG_XB1_PLATFORM && !_EG_SWITCH_PLATFORM && !defined _EG_GAMECORE_PLATFORM
int main(int argc, const char* argv[])
{
	ui.main();
	return 0;
}
#endif