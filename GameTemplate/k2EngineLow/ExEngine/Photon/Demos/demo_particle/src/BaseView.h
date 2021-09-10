#pragma once

#include "Common-cpp/inc/Common.h"

class BaseView
{
public:
	virtual ~BaseView(void) {}
	virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::Common::JString& joinedRoomName) = 0;
	virtual void initPlayers(void) {}
	virtual void addPlayer(int playerNr, const ExitGames::Common::JString& playerName, bool local) = 0;
	virtual void removePlayer(int playerNr) = 0;
	virtual void changePlayerColor(int playerNr, int color) = 0;
	virtual void changePlayerPos(int playerNr, int x, int y) = 0;
    virtual void setupScene(int gridSize) = 0;
	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames) = 0;
};