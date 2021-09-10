#pragma once

#include "Common-cpp/inc/Common.h"

class BaseView : private ExitGames::Common::BaseListener
{
public:
	BaseView(void);
	virtual ~BaseView(void);

	virtual void onStateChange(int state, const ExitGames::Common::JString& stateStr, bool inLobby, bool inRoom);
	virtual void refreshStateInfo(const ExitGames::Common::JString& stateInfo);
	virtual void refreshMessage(const ExitGames::Common::JString& msg);

	virtual void setup(void);
	virtual void setupBoard(int tileCols, int tileRows);
	virtual void updateBoard(unsigned char* tiles, unsigned char flippedTiles[2]);
	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);
	virtual void updateSavedRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& savedRoomNames);
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);
protected:
	ExitGames::Common::Logger mLogger;
};