#include <stdarg.h>
#include <stdio.h>

#include "ConsoleView.h"
#include "Console.h"

#ifdef _EG_ANDROID_PLATFORM
#	include <android/log.h>
#endif

using namespace ExitGames::Common;

ConsoleView::~ConsoleView(void)
{
}

void ConsoleView::updateState(int state, const JString& stateStr, const JString& joinedRoomName) 
{
	Console::get().writeLine(JString(L"state: ") + state + L"/" + stateStr);
}

void ConsoleView::addPlayer(int playerNr, const JString& playerName, bool local) 
{
	Console::get().writeLine(JString(L"player added: ") + playerName + L" / " + playerNr);
}

void ConsoleView::removePlayer(int playerNr) 
{
	Console::get().writeLine(JString(L"player removed: ") + playerNr);
}

void ConsoleView::changePlayerColor(int playerNr, int color) 
{
	Console::get().writeLine(JString(L"player ") + playerNr + L" color: " + color);
}

void ConsoleView::changePlayerPos(int playerNr, int x, int y) 
{
	Console::get().writeLine(JString(L"player ") + playerNr + L" pos: " + x + L", " + y);
}

void ConsoleView::setupScene(int gridSize)
{
	Console::get().writeLine(JString(L"setup scene with gridSize=") + gridSize);
}

void ConsoleView::updateRoomList(const JVector<JString>& roomNames) 
{
	Console::get().writeLine(L"room list:");
	for(unsigned int i=0; i<roomNames.getSize(); ++i) 
		Console::get().writeLine(roomNames[i]);
}