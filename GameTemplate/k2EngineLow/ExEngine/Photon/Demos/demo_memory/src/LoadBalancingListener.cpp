#include <stdlib.h>
#include "cocos2d.h"
#include "Console.h"
#include "DemoConstants.h"
#include "Game.h"
#include "LoadBalancingListener.h"

#ifdef _EG_ANDROID_PLATFORM
#	include <string.h>
#	include <stdio.h>
#	include <jni.h>
//	void parsePushSubscribeToUserChannel(const ExitGames::Common::JString& userID);
#else
#	include <iostream>
#endif

using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

const JString PeerStatesStr[] =
{
	L"Uninitialized",
	L"PeerCreated",
	L"ConnectingToNameserver",
	L"ConnectedToNameserver",
	L"DisconnectingFromNameserver",
	L"Connecting",
	L"Connected",
	L"WaitingForCustomAuthenticationNextStepCall",
	L"Authenticated",
	L"JoinedLobby",
	L"DisconnectingFromMasterserver",
	L"ConnectingToGameserver",
	L"ConnectedToGameserver",
	L"AuthenticatedOnGameServer",
	L"Joining",
	L"Joined",
	L"Leaving",
	L"Left",
	L"DisconnectingFromGameserver",
	L"ConnectingToMasterserver",
	L"ConnectedComingFromGameserver",
	L"AuthenticatedComingFromGameserver",
	L"Disconnecting",
	L"Disconnected"
};

// TODO: update PeerStatesStr when PeerStates changes
// Checker below checks count match only
class PeerStatesStrChecker
{
public:
	PeerStatesStrChecker(void)
	{
		assert(sizeof(PeerStatesStr)/sizeof(JString) == PeerStates::Disconnected + 1);
	}
} checker;

LoadBalancingListener::LoadBalancingListener(void) 
	: mpLbc(NULL)
	, mpView(NULL)
	, mpGame(NULL)
{
	mLogger.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
	mLogger.setListener(*this);
}

LoadBalancingListener::~LoadBalancingListener(void)  
{ 
	delete mpGame; 
}

void LoadBalancingListener::init(ExitGames::LoadBalancing::Client* pLbc, BaseView* pView)
{
	this->mpLbc = pLbc;
	this->mpView = pView;
	mpGame = new Game(this, pView);
	updateState();
}

void LoadBalancingListener::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
}

ExitGames::LoadBalancing::Client* LoadBalancingListener::getClient()
{
	return mpLbc;
}

void LoadBalancingListener::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void LoadBalancingListener::connectionErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"connection failed with error %d", errorCode);
	updateState();
}

void LoadBalancingListener::clientErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"received error %d from client", errorCode);
	updateState();
}

void LoadBalancingListener::warningReturn(int warningCode)
{
	EGLOG(DebugLevel::WARNINGS, L"received warning %d from client", warningCode);
}

void LoadBalancingListener::serverErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"received error %d from server", errorCode);
	updateState();
}

void LoadBalancingListener::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player)
{
	EGLOG(DebugLevel::INFO, L"player %d %ls joined the game", playerNr, player.getName().cstr());
	mpGame->onActorJoin();
	mpGame->refreshInfo();

	// saving names
	JString names;
	for(int i=0; i<getClient()->getCurrentlyJoinedRoom().getPlayerCount(); ++i) 
	{
		if(names.length())
			names += ";";
		names += getClient()->getCurrentlyJoinedRoom().getPlayers()[i]->getName();
	}
	getClient()->getCurrentlyJoinedRoom().addCustomProperty(PROP_NAMES, names);
}

void LoadBalancingListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(DebugLevel::INFO, L"player %d %ls", playerNr, isInactive ? L"suspended the game" : L"abandoned the game");
	if(!isInactive)
		mpGame->onActorAbandon(playerNr);
	mpGame->refreshInfo();
}

void LoadBalancingListener::connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster)
{
	updateState();
    if(errorCode == ErrorCode::OK)
		EGLOG(DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
	else
		EGLOG(DebugLevel::WARNINGS, L"Warn: connect failed %d %ls", errorCode, errorString.cstr());
}

void LoadBalancingListener::disconnectReturn(void)
{
	updateState();
	EGLOG(DebugLevel::INFO, L"disconnected");
}

void LoadBalancingListener::createRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		EGLOG(DebugLevel::INFO, L"room created");
		afterRoomJoined(localPlayerNr, gameProperties);
	}
	else
		EGLOG(DebugLevel::WARNINGS, L"Warn: opCreateRoom() failed: %ls", errorString.cstr());
}

void LoadBalancingListener::joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		EGLOG(DebugLevel::INFO, L"room has been entered");
		afterRoomJoined(localPlayerNr, gameProperties);
	}
	else
		EGLOG(DebugLevel::WARNINGS, L"Warn: opJoinOrCreateRoom() failed: %ls", errorString.cstr());
}

void LoadBalancingListener::joinRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		EGLOG(DebugLevel::INFO, L"game room joined");
        afterRoomJoined(localPlayerNr, gameProperties);
	}
	else
		EGLOG(DebugLevel::INFO, L"opJoinRoom() failed: %ls", errorString.cstr());
}

void LoadBalancingListener::joinRandomRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::NO_MATCH_FOUND)
		EGLOG(DebugLevel::INFO, L"no match found for random room");
    else if(errorCode == ErrorCode::OK)
	{
		EGLOG(DebugLevel::INFO, L"game room joined");
        afterRoomJoined(localPlayerNr, gameProperties);
	}
	else
		EGLOG(DebugLevel::INFO, L"opJoinRandomRoom() failed: %ls", errorString.cstr());
}

void LoadBalancingListener::leaveRoomReturn(int errorCode, const JString& errorString)
{
	updateState();
        
	if(errorCode == ErrorCode::OK)
		EGLOG(DebugLevel::INFO, L"game room left");
	else
		EGLOG(DebugLevel::INFO, L"opLeaveRoom() failed: %ls", errorString.cstr());
}

void LoadBalancingListener::gotQueuedReturn(void)
{
	updateState();
}

void LoadBalancingListener::joinLobbyReturn(void)
{
	EGLOG(DebugLevel::INFO, L"joined lobby");
	updateState();
//	mpLbc->opJoinRandomRoom();
}

void LoadBalancingListener::leaveLobbyReturn(void)
{
	EGLOG(DebugLevel::INFO, L"left lobby");
	updateState();
}

void LoadBalancingListener::onRoomListUpdate()
{	
	const JVector<Room*>& rooms = mpLbc->getRoomList();
	JVector<JString> names(rooms.getSize());
	for(unsigned int i=0; i<rooms.getSize(); ++i)
		names.addElement(rooms[i]->getName());
	mpView->updateRoomList(names);
}

void LoadBalancingListener::onRoomPropertiesChange(const Hashtable& changes) 
{
	mpGame->updateStateFromProps(changes);
}

void LoadBalancingListener::webRpcReturn(int errorCode, const JString& errorString, const JString& uriPath, int resultCode, const Dictionary<Object, Object>& data)
{
	if(!errorCode&& !resultCode)
	{
		JVector<JString> names(data.getSize());
		const JVector<Object>& keys = data.getHashtable().getKeys(); // keys are saved rooms names
		mSavedRoomList.removeAllElements();
		for(unsigned int i=0; i<keys.getSize(); i++)
		{
			JString name = ((ValueObject<JString>)keys[i]).getDataCopy();
			Dictionary<Object, Object> info = ((ValueObject<Dictionary<Object, Object> >)data.getHashtable().getValue(keys[i])).getDataCopy();

			mSavedRoomList.put(name, info);
			names.addElement(name);
		}
		mpView->updateSavedRoomList(names);
	}
}

void LoadBalancingListener::connect(const JString& userName) 
{ 
	EGLOG(DebugLevel::INFO, L"Connecting...");
#ifdef _EG_ANDROID_PLATFORM
//	parsePushSubscribeToUserChannel(userName);
#endif
	mpLbc->getLocalPlayer().setName(userName);
	mpLbc->connect(AuthenticationValues().setUserID(JString()+GETTIMEMS()));
}

void LoadBalancingListener::updateState() 
{
	int state = mpLbc->getState();
	if(mpLbc->getIsInLobby())
		querySavedRoomList();

	mpView->onStateChange(state, PeerStatesStr[state], mpLbc->getIsInLobby(), mpLbc->getIsInGameRoom());
	mpGame->refreshInfo();
}

void LoadBalancingListener::afterRoomJoined(int localPlayerNr, const Hashtable& gameProperties)
{
	EGLOG(DebugLevel::INFO, L"afterRoomJoined: localPlayerNr=", localPlayerNr);
	mpGame->updateStateFromProps(gameProperties);
	mpGame->onActorJoin();
	mpGame->refreshInfo();

	mpView->setupBoard(BOARD_SIZE, BOARD_SIZE);
	mpGame->updateBoard();

	if(mpLbc->getCurrentlyJoinedRoom().getPlayerCount() == 2)
	{
		mpLbc->getCurrentlyJoinedRoom().setIsVisible(false); // hide room when it's full
		mpLbc->getCurrentlyJoinedRoom().setIsOpen(false); // hide room when it's full
	}
}

void LoadBalancingListener::createRoom(const JString& userName) 
{
	mpLbc->getLocalPlayer().setName(userName);
	JString roomName = userName + L"-" + (rand()%1000);

	// reset all data
	delete mpGame;
	mpGame = new Game(this, mpView);
	mpGame->generateNewBoard();
	Hashtable props = mpGame->stateToProps(false);
	JVector<JString> propsListedInLobby;
	propsListedInLobby.addElement(PROP_TURN);
	propsListedInLobby.addElement(PROP_NAMES);

	mpLbc->opCreateRoom(roomName, RoomOptions().setMaxPlayers(MAX_PLAYERS).setCustomRoomProperties(props).setPropsListedInLobby(propsListedInLobby).setPlayerTtl(INT_MAX).setEmptyRoomTtl(5000));
	EGLOG(DebugLevel::INFO, L"Creating room %ls", roomName.cstr());
}

void LoadBalancingListener::service()
{
	mpGame->service();	
}
		   
void LoadBalancingListener::click(int tileIdx)
{
	mpGame->click(tileIdx);
}
	
void LoadBalancingListener::querySavedRoomList() 
{
	mpLbc->opWebRpc(L"GetGameList");
}

void LoadBalancingListener::joinRoom(const JString& gameId)
{
	EGLOG(DebugLevel::INFO, L"Joining Room: %ls", gameId.cstr());
	mpLbc->opJoinRoom(gameId);
}

void LoadBalancingListener::joinRandomRoom(void)
{
	EGLOG(DebugLevel::INFO, L"Joining Random Room");
	mpLbc->opJoinRandomRoom();
}

void LoadBalancingListener::suspendRoom(void)
{
	EGLOG(DebugLevel::INFO, L"Suspending");
	mpLbc->opLeaveRoom(true);
}

void LoadBalancingListener::abandonRoom(void)
{
	EGLOG(DebugLevel::INFO, L"Abandoning");
	mpLbc->opLeaveRoom();
}

void LoadBalancingListener::joinSavedRoom(const JString& gameId)
{
	const Dictionary<Object, Object>* info = mSavedRoomList.getValue(gameId);
	if(info)
	{
		const Object* actorNrObj = info->getHashtable().getValue(L"ActorNr");
		if(actorNrObj)
		{
			EGLOG(DebugLevel::INFO, L"Joining Saved Room: %ls", gameId.cstr());
			mpLbc->opJoinRoom(gameId, true);
			return;
		}
	}

	EGLOG(DebugLevel::ERRORS, L"No saved room for id", gameId.cstr());
}

int LoadBalancingListener::getNextPlayer(int fromPlayer)
{
	int lowest = fromPlayer;
	int next = INT_MAX;
	JVector<Player*> const& players = mpLbc->getCurrentlyJoinedRoom().getPlayers();
	for(unsigned i=0; i<players.getSize(); i++)
	{
		int nr = players[i]->getNumber();
		if(nr > fromPlayer && nr < next)
			next = nr;
		if(nr < lowest)
			lowest = nr;
	}
	if(next == INT_MAX)
		return lowest;
	else
		return next;
}

bool LoadBalancingListener::leave()
{
	if(mpLbc->getIsInLobby())
	{
		mpLbc->disconnect();
		return true;
	}
	else
		return false;
}