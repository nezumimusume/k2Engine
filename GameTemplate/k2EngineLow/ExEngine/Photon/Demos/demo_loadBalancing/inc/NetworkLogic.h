#pragma once

#include "OutputListener.h"
#include "LoadBalancing-cpp/inc/Client.h"
#ifdef _EG_PS4_OR_NEWER_PLATFORM
#	include "AuthenticationParameters.h"
#endif

enum State
{
	STATE_INITIALIZED = 0,
	STATE_CONNECTING,
	STATE_CONNECTED,
	STATE_JOINING,
	STATE_JOINED,
	STATE_LEAVING,
	STATE_LEFT,
	STATE_DISCONNECTING,
	STATE_DISCONNECTED
};

enum Input
{
	INPUT_NON = 0,
	INPUT_1,
	INPUT_2,
	INPUT_3,
	INPUT_4,
	INPUT_EXIT
};

class NetworkLogicListener : public ExitGames::Common::ToString
{
public:
	using ToString::toString;
	virtual void stateUpdate(State newState) = 0;
	virtual ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes=false) const;
};

class StateAccessor
{
public:
	State getState(void) const;
	void setState(State newState);
	void registerForStateUpdates(NetworkLogicListener* listener);

private:
	State mState;
	ExitGames::Common::JVector<NetworkLogicListener*> mStateUpdateListeners;
};

class NetworkLogic : private ExitGames::LoadBalancing::Listener
{
public:
	NetworkLogic(OutputListener* listener);
	void registerForStateUpdates(NetworkLogicListener* listener);
	void run(void);
	void connect(void);
	void opCreateRoom(nByte directMode);
	void opJoinOrCreateRoom(void);
	void opJoinRandomOrCreateRoom(void);
	void opJoinRoom(const ExitGames::Common::JString& roomID, bool rejoin=false);
	void opJoinRandomRoom(void);
	void disconnect(void);
	void sendEvent(void);

	Input getLastInput(void) const;
	void setLastInput(Input newInput);
	State getState(void) const;

#if defined _EG_MICROSOFT_CONSOLE_PLATFORM
	void setXSTSToken(const ExitGames::Common::JVector<nByte>& mXSTSToken);
#elif defined _EG_SWITCH_NX_PLATFORM
	void setIDToken(const ExitGames::Common::JString& IDToken);
#endif
private:
	void sendDirect(int64 count);

	// receive and print out debug out here
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);

	// events, triggered by certain operations of all players in the same room
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr, bool isInactive);
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

	virtual void onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
	virtual void onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
	virtual void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers);
	virtual void onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay);
	// callbacks for operations on PhotonLoadBalancing server
	virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster);
	virtual void disconnectReturn(void);
	virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRandomOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinLobbyReturn(void);
	virtual void leaveLobbyReturn(void);

	OutputListener* mpOutputListener;
	ExitGames::LoadBalancing::Client mLoadBalancingClient;
	ExitGames::Common::JString mLastJoinedRoom;
	ExitGames::Common::JString mUserID;
	ExitGames::Common::Logger mLogger;
	StateAccessor mStateAccessor;
	Input mLastInput;
	bool mAutoJoinRoom;
	ExitGames::Common::JTime mLastSendTime;
#if defined _EG_MICROSOFT_CONSOLE_PLATFORM
	ExitGames::Common::JVector<nByte> mXSTSToken;
	bool mReauthenticateRequired;
#elif defined _EG_SWITCH_NX_PLATFORM
	ExitGames::Common::JString mIDToken;
#endif
};