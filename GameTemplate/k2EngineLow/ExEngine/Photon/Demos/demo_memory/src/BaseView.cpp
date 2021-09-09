#include <stdarg.h>
#include <stdio.h>

#include "BaseView.h"
#include "Console.h"

#ifdef _EG_ANDROID_PLATFORM
#	include <android/log.h>
#endif

using namespace ExitGames::Common;

BaseView::BaseView(void)
{
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
}

BaseView::~BaseView(void)
{
}

void BaseView::onStateChange(int state, const JString& stateStr, bool inLobby, bool inRoom) 
{
	EGLOG(DebugLevel::INFO, L"state: %d/%ls", state, stateStr.cstr());
}

void BaseView::refreshStateInfo(const JString& stateInfo)
{
	EGLOG(DebugLevel::INFO, L"state info: %ls", stateInfo.cstr());
}

void BaseView::refreshMessage(const JString& msg)
{
	EGLOG(DebugLevel::INFO, L"game message: %ls", msg.cstr());
}

void BaseView::setup(void)
{
	EGLOG(DebugLevel::INFO, L"setup view");
}

void BaseView::setupBoard(int tileCols, int tileRows)
{
	EGLOG(DebugLevel::INFO, L"setup board %dx%d", tileCols, tileRows);
}

void BaseView::updateBoard(unsigned char* tiles, unsigned char flippedTiles[2])
{
	EGLOG(DebugLevel::INFO, L"update board, flips: [%d, %d]", flippedTiles[0], flippedTiles[1]);
}

void BaseView::updateRoomList(const JVector<JString>& roomNames) 
{
	EGLOG(DebugLevel::INFO, L"room list:");
	for(unsigned int i=0; i<roomNames.getSize(); ++i) 
		EGLOG(DebugLevel::INFO, L"  %ls", roomNames[i].cstr());
}

void BaseView::updateSavedRoomList(const JVector<JString>& savedRoomNames) 
{
	EGLOG(DebugLevel::INFO, L"saved room list:");
	for(unsigned int i=0; i<savedRoomNames.getSize(); ++i)
		EGLOG(DebugLevel::INFO, L"  %ls", savedRoomNames[i].cstr());
}

void BaseView::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}