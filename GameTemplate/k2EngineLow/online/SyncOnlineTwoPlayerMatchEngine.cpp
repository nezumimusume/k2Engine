#include "k2EngineLowPreCompile.h"
#include "online/SyncOnlineTwoPlayerMatchEngine.h"

namespace nsK2EngineLow {
	namespace {
		const ExitGames::Common::JString PLAYER_NAME = L"user";
	}

	SyncOnlineTwoPlayerMatchEngine::~SyncOnlineTwoPlayerMatchEngine()
	{
		m_loadBalancingClient->opLeaveRoom();
		m_loadBalancingClient->opLeaveLobby();
		m_loadBalancingClient->disconnect();
	}
	void SyncOnlineTwoPlayerMatchEngine::Init(
		const wchar_t* appID,
		const wchar_t* appVersion,
		void* pSendData,
		int sendDataSize,
		std::function<void(void* pRecvData, int dataSize)> onAllPlayerJoinedRoom,
		std::function<void()> onAllPlayerPossibleGameStart,
		std::function<void()> onError
	)
	{
		m_allPlayerNotifyPossibleGameStartFunc = onAllPlayerPossibleGameStart;
		m_allPlayerJoinedRoomFunc = onAllPlayerJoinedRoom;
		m_errorFunc = onError;
		ExitGames::LoadBalancing::Client* p = new ExitGames::LoadBalancing::Client(
			*this,
			appID,
			appVersion,
			ExitGames::Photon::ConnectionProtocol::DEFAULT,
			true
		);
		m_loadBalancingClient.reset(p);
		
		m_loadBalancingClient->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
		m_logger.setListener(*this);
		m_logger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
		ExitGames::Common::Base::setListener(this);
		ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
		m_isInited = true;

	}
	void SyncOnlineTwoPlayerMatchEngine::SendJoined()
	{
		// ルームにジョインしたことを通知。
		ExitGames::LoadBalancing::RaiseEventOptions eventOpt;
		ExitGames::Common::Hashtable event;
		m_loadBalancingClient->opRaiseEvent(
			true,
			event,
			Event_JoinedOtherPalyer,
			eventOpt
		);
	}
	void SyncOnlineTwoPlayerMatchEngine::SendPossibleGameStart()
	{
		// ゲーム開始ベントを送信。
		ExitGames::LoadBalancing::RaiseEventOptions eventOpt;

		ExitGames::Common::Hashtable event;

		m_loadBalancingClient->opRaiseEvent(
			true,
			event,
			Event_PossibleGameStartOtherPlayer,
			eventOpt
		);
	}
	void SyncOnlineTwoPlayerMatchEngine::SendPadData()
	{
		// 送るパッドデータを構築する。
		SPadData padData;
		padData.dataType = 0;
		padData.xInputState = g_pad[0]->GetXInputState();
		padData.frameNo = m_frameNo;
		m_padData[0].insert({ m_frameNo, padData });

		m_loadBalancingClient->sendDirect(
			(std::uint8_t*)&padData,
			sizeof(padData)
		);
	}
	void SyncOnlineTwoPlayerMatchEngine::RequestResendPadData(int frameNo)
	{
		SRequestResendPadData reqResendPadData;
		reqResendPadData.dataType = 1;
		reqResendPadData.frameNo = frameNo;

		m_loadBalancingClient->sendDirect(
			(std::uint8_t*)&reqResendPadData,
			sizeof(reqResendPadData)
		);
	}
	void SyncOnlineTwoPlayerMatchEngine::Update()
	{
		if (!m_isInited) {
			return;
		}
		m_loadBalancingClient->service();

		switch (m_state) {
		case State::INITIALIZED: {
			ExitGames::LoadBalancing::ConnectOptions connectOption;
			connectOption.setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS())).setUsername(PLAYER_NAME + GETTIMEMS());
			connectOption.setTryUseDatagramEncryption(true);
			//connectOption.set
			m_loadBalancingClient->connect(connectOption	);
			m_state = State::CONNECTING;
		}break;
		case State::CONNECTED: {
			ExitGames::LoadBalancing::RoomOptions roomOption;
			roomOption.setMaxPlayers(2);
			roomOption.setDirectMode(ExitGames::LoadBalancing::DirectMode::ALL_TO_ALL);
			m_loadBalancingClient->opJoinOrCreateRoom(L"Test", roomOption);
			m_timer = 0.0f;
			m_state = State::JOINING;
		}break;
		case State::JOINED:
			// ルームに入ったことを他プレイヤーに通知。
			
			if (m_isJoinedOtherPlayer) {
				// すべてのプレイヤーがルームにそろったこと。
				SendJoined();
				
				m_timer = 0.0f;
				m_state = State::WAIT_START_GAME;
			}
			break;
		case State::WAIT_START_GAME:
			if (m_isPossibleGameStart
				&& m_isPossibleGameStartOtherPlayer) {
				m_frameNo++;
				m_allPlayerNotifyPossibleGameStartFunc();
				m_state = State::IN_GAME_BUFFERING_PAD_DATA;
			}
			
			break;
		case State::IN_GAME_BUFFERING_PAD_DATA:
			// パッドデータを送信。
			if (m_frameNo < 3) {
				// 3フレーム分だけ送る。
				SendPadData();
				m_frameNo++;
			}
			else {
				// 3フレーム遅延させてスタート。
				m_state = State::IN_GAME;
			}
			
			break;
		case State::IN_GAME: {
			int loopCount = 0;
			while(true) {	
				auto it = m_padData[1].find(m_playFrameNo);
				if (it != m_padData[1].end()) {
					// 再生フレームのパッド情報を受け取っている。
					m_pad[0].Update(m_padData[0][m_playFrameNo].xInputState);
					m_pad[1].Update(it->second.xInputState);
					// 再生済みのパッド情報を削除。
					m_padData[0].erase(m_playFrameNo);
					m_padData[1].erase(m_playFrameNo);
					break;
				}
				else {
					// データが来ていない？
					// UDPなのでパケットロストしている可能性があるので、再送リクエストを送る。
					RequestResendPadData(m_playFrameNo);
					loopCount++;
					Sleep(100);
					m_loadBalancingClient->service();
					if (loopCount == 100) {
						// 接続エラー。
						Disconnect();
						break;
					}
				}
			} 
			SendPadData();
			m_playFrameNo++;
			m_frameNo++;
		}break;
		case State::DISCONNECTING:
			break;
		default:
			break;
		}
	}
	void SyncOnlineTwoPlayerMatchEngine::Disconnect()
	{
		m_errorFunc();
		m_loadBalancingClient->disconnect();
		m_state = State::DISCONNECTING;
	}
	void SyncOnlineTwoPlayerMatchEngine::onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay)
	{
		auto valueObj = (ExitGames::Common::ValueObject<std::uint8_t*>*)&msg;
		const int* sizes = valueObj->getSizes();
		std::uint8_t* pData = (std::uint8_t*)valueObj->getDataCopy();
		int dataType = (int)(*pData);
		switch (dataType) {
		case 0: {
			// パッド情報
			SPadData padData;
			memcpy(&padData, pData, sizes[0]);
			m_padData[1].insert({ padData.frameNo, padData });
		}break;
		case 1: {
			// パッドデータの再送リクエスト
			SRequestResendPadData reqResendPadData;
			memcpy(&reqResendPadData, pData, sizes[0]);
			
			// 送るパッドデータを構築する。
			SPadData padData;
			padData.dataType = 0;
			padData.xInputState = m_padData[0][reqResendPadData.frameNo].xInputState;
			padData.frameNo = m_padData[0][reqResendPadData.frameNo].frameNo;
			m_padData[0].insert({ padData.frameNo, padData });

			m_loadBalancingClient->sendDirect(
				(std::uint8_t*)&padData,
				sizeof(padData)
			);

		}break;
		}
		
	}

	void SyncOnlineTwoPlayerMatchEngine::leaveRoomEventAction(int playerNr, bool isInactive)
	{
		// 部屋からプレイヤーが抜けたので、ゲーム終了。
		Disconnect();
	}
	
	void SyncOnlineTwoPlayerMatchEngine::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
	{
		ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
		switch (eventCode) {
		case Event_JoinedOtherPalyer:

			m_allPlayerJoinedRoomFunc(m_recieveDataOnGameStart.get(), m_recieveDataSize);
			m_state = WAIT_START_GAME;
			break;
		case Event_PossibleGameStartOtherPlayer:
			m_isPossibleGameStartOtherPlayer = true;
			break;
		}
	}
	// callbacks for operations on PhotonLoadBalancing server
	void SyncOnlineTwoPlayerMatchEngine::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
	{
		if (errorCode)
		{
			// 入れなかった。
		//	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
			Disconnect();
			return;
		}
		// 部屋に入れた。
		m_state = State::CONNECTED;
	}
	void SyncOnlineTwoPlayerMatchEngine::disconnectReturn(void)
	{
		// 接続済みにする。
		m_state = State::DISCONNECTED;
	}
	
	void SyncOnlineTwoPlayerMatchEngine::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
	{
		if (errorCode) {
			// 部屋を作れなかった。
			// ステータスを接続済みに戻して、再度部屋を作る。
			m_state = State::CONNECTED;
			return;
		}
		if (localPlayerNr == 1) {
			// 部屋を作ったホスト。
			m_playerType = PlayerType_Host;
		}
		else {
			// クライアント。
			m_isJoinedOtherPlayer = true;
			m_playerType = PlayerType_Client;
		}
		// ルームに入った。
		m_state = State::JOINED;
	}
	void SyncOnlineTwoPlayerMatchEngine::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
	{
		if (m_playerType == PlayerType_Host
			&& playerNr == 2
		) {
			m_isJoinedOtherPlayer = true;
		}
	}
}