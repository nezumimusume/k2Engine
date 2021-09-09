#include "k2EngineLowPreCompile.h"
#include "online/LobbyManager.h"

namespace nsK2EngineLow {
	LobbyManager::LobbyManager(
		const wchar_t* appID,
		const wchar_t* appVersion
	) :
		m_mLoadBalancingClient(*this, appID, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, true)
	{

	}
	void LobbyManager::Update()
	{

	}
	void LobbyManager::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
	{

	}
	// implement your error-handling here
	void LobbyManager::connectionErrorReturn(int errorCode)
	{

	}
	void LobbyManager::clientErrorReturn(int errorCode)
	{

	}
	void LobbyManager::warningReturn(int warningCode)
	{

	}
	void LobbyManager::serverErrorReturn(int errorCode)
	{

	}
	// events, triggered by certain operations of all players in the same room
	void LobbyManager::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
	{

	}
	void LobbyManager::leaveRoomEventAction(int playerNr, bool isInactive)
	{

	}
	void LobbyManager::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
	{

	}
	// callbacks for operations on PhotonLoadBalancing server
	void LobbyManager::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
	{

	}
	void LobbyManager::disconnectReturn(void)
	{

	}
	void LobbyManager::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
	{

	}
	void LobbyManager::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
	{

	}
	void LobbyManager::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
	{

	}
	void LobbyManager::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
	{

	}
	void LobbyManager::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
	{

	}
	void LobbyManager::joinLobbyReturn(void)
	{

	}
	void LobbyManager::leaveLobbyReturn(void)
	{

	}
}