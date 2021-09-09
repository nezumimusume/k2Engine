#include "Game.h"
#include "DemoConstants.h"
#include <cstdio>
#include <stdlib.h>

using namespace ExitGames::Common;

Game::Game(LoadBalancingListener* pLbl, BaseView* pView)
	: mpLbl(pLbl)
	, mpView(pView)
	, mNextTurnPlayer(0)
	, mTurnNumber(1)
	, mMyPoints(0)
	, mOthersPoints(0)
	, mAbandoned(false)
	, mEndTurnDelayedRunAt(0)
	
{
	resetFlippedTiles();
	for(int i = 0;i < TILE_COUNT;++i)
		mTiles[i] = 0;
}

Game::~Game(void)
{
}

void Game::resetFlippedTiles(void)
{
	mFlippedTiles[0] = NOT_FLIPPED_TILE;
	mFlippedTiles[1] = NOT_FLIPPED_TILE;
}

void Game::service(void)
{
	unsigned long t = GETTIMEMS();
	if(mEndTurnDelayedRunAt && t >= mEndTurnDelayedRunAt) 
	{
		mEndTurnDelayedRunAt = 0;
		endTurnDelayed();
	}
}

bool Game::logic(void)
{
	bool turnPass = false;
	if(mFlippedTiles[0] != NOT_FLIPPED_TILE && mFlippedTiles[1] != NOT_FLIPPED_TILE)
		turnPass = endTurn();
	updateBoard();
	refreshInfo();
	return turnPass;
}

void Game::updateStateFromProps(const ExitGames::Common::Hashtable& changedCustomProps)
{
	JString buf;
	for(int i = 0; i<TILE_COUNT; ++i)
	{
		Object const* v = changedCustomProps.getValue(JString()+i);
		if(v)
			mTiles[i] = ((ValueObject<nByte>*)v)->getDataCopy();
	}
	Object const* v = changedCustomProps.getValue("flips");
	if(v)
	{
		if(v->getDimensions() == 1 && v->getSizes()[0] == 2) 
		{
			nByte* data = ((ValueObject<nByte*>*)v)->getDataCopy();
			mFlippedTiles[0] = data[0];
			mFlippedTiles[1] = data[1];
		}
		else
			EGLOG(DebugLevel::WARNINGS, L"Bad flips incoming data");
	}
	
	v = changedCustomProps.getValue(L"pt");
	if(v)
		mNextTurnPlayer = ((ValueObject<int>*)v)->getDataCopy();
	v = changedCustomProps.getValue(L"t#");
	if(v)
		mTurnNumber = ((ValueObject<int>*)v)->getDataCopy();

	v = changedCustomProps.getValue(JString(L"pt")+myActorNr());
	if(v)
		mMyPoints = ((ValueObject<nByte>*)v)->getDataCopy();
	v = changedCustomProps.getValue(L"mAbandoned");
	if(v)
		mAbandoned = ((ValueObject<bool>*)v)->getDataCopy();

	int otherId = mpLbl->getNextPlayer(myActorNr());
	if(otherId != myActorNr())
	{
		v = changedCustomProps.getValue(JString(L"pt")+otherId);
		if(v)
			mOthersPoints = ((ValueObject<nByte>*)v)->getDataCopy();
	}

	logic();
}

void Game::saveStateToProps(bool turnPass)
{
	const Hashtable& props = stateToProps(turnPass);
	mpLbl->getClient()->getCurrentlyJoinedRoom().mergeCustomProperties(props, Hashtable(), turnPass);
}
	
Hashtable Game::stateToProps(bool turnPass)
{
	Hashtable props;
	
    for(int i=0; i<TILE_COUNT; ++i)
	{		
		if(mTiles[i])
			props.put(JString()+i, mTiles[i]);
	}
	props.put(L"flips", mFlippedTiles, 2);
    props.put(L"pt", mNextTurnPlayer);
    props.put(L"t#", mTurnNumber);
	props.put(JString(L"pt")+myActorNr(), mMyPoints); // we always only save "our" points. this will not affect the opponent's score.
	if(mAbandoned)
		props.put(L"mAbandoned", mAbandoned);
	if(turnPass)
		props.put(PROP_TURN, mpLbl->getNextPlayer(mNextTurnPlayer));
				
	return props;
}

bool Game::endTurn(void)
{
	EGLOG(DebugLevel::INFO, L"EndTurn");

	bool turnPass = false;
	DEBUG_ASSERT(mFlippedTiles[0] != NOT_FLIPPED_TILE);
	DEBUG_ASSERT(mFlippedTiles[1] != NOT_FLIPPED_TILE);
		
	if(mTiles[mFlippedTiles[0]] == mTiles[mFlippedTiles[1]])
		EGLOG(DebugLevel::INFO, L"Match!");
	else
	{
		EGLOG(DebugLevel::INFO, L"Miss. Next player:", mNextTurnPlayer);
		turnPass = true;
	}

	refreshInfo();
	
	mEndTurnDelayedRunAt = GETTIMEMS() + END_OF_TURN_DELAY;

	return turnPass;
}
	
void Game::endTurnDelayed(void)
{
	if(mTiles[mFlippedTiles[0]] == mTiles[mFlippedTiles[1]])
	{
		mTiles[mFlippedTiles[0]] = 0;
		mTiles[mFlippedTiles[1]] = 0;
		if(mNextTurnPlayer == myActorNr())
			++mMyPoints;
		else
			++mOthersPoints;
	}
	else
		mNextTurnPlayer = mpLbl->getNextPlayer(mNextTurnPlayer);

	resetFlippedTiles();
	updateBoard();
	refreshInfo();
}

void Game::generateNewBoard(void)
{
	for(int i=0; i<TILE_COUNT; ++i)
		 mTiles[i] = i % TILES_TYPE_COUNT + 1;
	for(int i=0; i<TILE_COUNT * 10; ++i)
	{
		int i1 = rand() % TILE_COUNT;
		int i2 = rand() % TILE_COUNT;
		nByte tmp = mTiles[i1];
		mTiles[i1] = mTiles[i2];
		mTiles[i2] = tmp;
	}
}
	
void Game::click(int tileIdx)
{
	EGLOG(DebugLevel::INFO, L"click");
	int otherId = mpLbl->getNextPlayer(myActorNr());
	if(otherId == myActorNr())
	{
		EGLOG(DebugLevel::INFO, L"Wait for other's join");
		return;
	}
	if(mNextTurnPlayer != myActorNr())
	{
		EGLOG(DebugLevel::INFO, L"Not your turn");
		return;
	}
		
	if(mFlippedTiles[0] != NOT_FLIPPED_TILE && mFlippedTiles[1] != NOT_FLIPPED_TILE)
	{
		EGLOG(DebugLevel::INFO, L"Please wait");
		return;
	}
	if(mTiles[tileIdx] != 0)
	{
		if(mFlippedTiles[0] == NOT_FLIPPED_TILE)
		{
			mFlippedTiles[0] = tileIdx;
			EGLOG(DebugLevel::INFO, L"flip 0");
		}
		else if(mFlippedTiles[1] == NOT_FLIPPED_TILE)
		{
			if(mFlippedTiles[0] != tileIdx) // check double click
			{
				mFlippedTiles[1] = tileIdx;
				EGLOG(DebugLevel::INFO, L"flip 1");
				if(mFlippedTiles[0] == mFlippedTiles[1])
					EGLOG(DebugLevel::INFO, L"Match");
			}
		}
		bool turnPass = logic();
		saveStateToProps(turnPass);
	}
}


bool Game::checkCompleted(JString& msg)
{
	msg = L"";
	for(int i=0; i<TILE_COUNT; ++i)
		if(mTiles[i] != 0)
			return false;

	if(mMyPoints > mOthersPoints)
		msg = L"You won!";
    else if(mMyPoints < mOthersPoints)
        msg = L"You lost!";
	else
		msg = L"Draw!";
	return true;
}
	
void Game::updateBoard(void)
{
	mpView->updateBoard(mTiles, mFlippedTiles);
}
	
JString Game::renderMessage(void)
{
	int otherId = mpLbl->getNextPlayer(myActorNr());
	JString complMsg;
	bool completed = checkCompleted(complMsg);
	if(mAbandoned)
		return L"Other left for good";
	else if(completed)
		return complMsg;
	else if(mNextTurnPlayer == otherId)
		return L"Other's turn";
	else if(mNextTurnPlayer == myActorNr())
		return L"Your turn";
	else if(mNextTurnPlayer == 0 || otherId == myActorNr())
		return L"Waiting for other's join";
	else
		return L"Wrong game state";
}

void Game::onActorJoin(void)
{
	if(mNextTurnPlayer == 0 && mpLbl->getClient()->getCurrentlyJoinedRoom().getPlayerCount() == 2)
		mNextTurnPlayer = mpLbl->getClient()->getCurrentlyJoinedRoom().getMasterClientID();
}
	
void Game::onActorAbandon(int playerNr)
{
	mAbandoned = true;
	saveStateToProps(false);
}
	

// updates state bar and game message
void Game::refreshInfo(void)
{
	if(mpLbl->getClient()->getIsInGameRoom()) // put some additional info in state bar
	{
		int otherNr = mpLbl->getNextPlayer(myActorNr());
		if(otherNr == myActorNr())
			otherNr = 0;
		bool otherInactive = false;
		if(otherNr)
		{
			const ExitGames::LoadBalancing::Player* o = mpLbl->getClient()->getCurrentlyJoinedRoom().getPlayerForNumber(otherNr);
			otherInactive = o && o->getIsInactive();
		}
		mpView->refreshStateInfo(mpLbl->getClient()->getCurrentlyJoinedRoom().getName() + L", " + myActorNr() + L" vs. " + (otherInactive?L"(":L"") + (otherNr?JString()+otherNr:JString()+L"?") + (otherInactive?L")":L"") + L", sc: " + mMyPoints + L"/" + mOthersPoints + L", t: " + mNextTurnPlayer);
	}
	else
		mpView->refreshStateInfo(L"");

	mpView->refreshMessage(renderMessage());
}

int Game::myActorNr(void)
{
	return mpLbl->getClient()->getLocalPlayer().getNumber();
}