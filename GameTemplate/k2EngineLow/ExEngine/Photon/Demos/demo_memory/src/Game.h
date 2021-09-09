
#pragma once

#include "LoadBalancingListener.h"
#include "BaseView.h"
#include "DemoConstants.h"

class Game 
{
public:
	Game(LoadBalancingListener* pLbl, BaseView* pView);
	~Game(void);

	void service(void);
	void click(int tileIdx);
	void updateStateFromProps(const ExitGames::Common::Hashtable& changedCustomProps);
	void refreshInfo(void);
	bool checkCompleted(ExitGames::Common::JString& msg);
	void onActorJoin(void);
	void onActorAbandon(int playerNr);
	void afterRoomJoinedvoid(void);
	void generateNewBoard(void);
	void updateBoard(void);
	void saveStateToProps(bool);
	ExitGames::Common::Hashtable stateToProps(bool turnPass);
private:
	ExitGames::Common::Logger mLogger;
	LoadBalancingListener* mpLbl;
	BaseView* mpView;

	nByte mTiles[TILE_COUNT];
	nByte mFlippedTiles[2];
	
	int mNextTurnPlayer;
	int mTurnNumber;
	nByte mMyPoints;
	int mOthersPoints;
	bool mAbandoned;

	unsigned long mEndTurnDelayedRunAt;

	bool logic(void);
	bool endTurn(void);
	void endTurnDelayed(void);
	int myActorNr(void);
	void resetFlippedTiles(void);
	ExitGames::Common::JString renderMessage(void);
};