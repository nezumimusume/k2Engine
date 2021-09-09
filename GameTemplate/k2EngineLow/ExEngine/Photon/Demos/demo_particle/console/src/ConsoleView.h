#pragma once

#include "../../src/BaseView.h"

class ConsoleView : public BaseView
{
public:
	virtual ~ConsoleView(void);

	virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::Common::JString& joinedRoomName);
	virtual void addPlayer(int playerNr, const ExitGames::Common::JString& playerName, bool local);
	virtual void removePlayer(int playerNr);
	virtual void changePlayerColor(int playerNr, int color);
	virtual void changePlayerPos(int playerNr, int x, int y);
	virtual void setupScene(int gridSize);
	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);
};