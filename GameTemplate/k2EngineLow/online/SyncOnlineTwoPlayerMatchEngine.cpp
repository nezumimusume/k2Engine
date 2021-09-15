#include "k2EngineLowPreCompile.h"
#include "online/SyncOnlineTwoPlayerMatchEngine.h"

#define ENABLE_ONLINE_DEBUG


#ifdef ENABLE_ONLINE_DEBUG
	#define ONLINE_LOG K2_LOG
	#define ONLINE_LOG_W K2_LOG_W
	// 有効でパッドのログを出力。
	//#define ENABLE_ONLINE_PAD_LOG
#else
	#define ONLINE_LOG
	#define ONLINE_LOG_W
#endif

namespace nsK2EngineLow {
	namespace {
		const ExitGames::Common::JString PLAYER_NAME = L"user";
	}
#ifdef ENABLE_ONLINE_PAD_LOG
	FILE* fp = nullptr;
#endif
	SyncOnlineTwoPlayerMatchEngine::~SyncOnlineTwoPlayerMatchEngine()
	{
		m_loadBalancingClient->opLeaveRoom();
		m_loadBalancingClient->opLeaveLobby();
		m_loadBalancingClient->disconnect();
#ifdef ENABLE_ONLINE_PAD_LOG
		fclose(fp);
#endif
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
#ifdef ENABLE_ONLINE_PAD_LOG
		fp = fopen("log.txt", "w");
#endif
		m_allPlayerNotifyPossibleGameStartFunc = onAllPlayerPossibleGameStart;
		m_allPlayerJoinedRoomFunc = onAllPlayerJoinedRoom;
		m_errorFunc = onError;
		ExitGames::LoadBalancing::Client* p = new ExitGames::LoadBalancing::Client(
			*this,
			appID,
			appVersion,
			ExitGames::Photon::ConnectionProtocol::TCP,
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
	void SyncOnlineTwoPlayerMatchEngine::SendInitDataOtherPlayer()
	{
		// ルームにジョインしたことを通知。
		ExitGames::LoadBalancing::RaiseEventOptions eventOpt;
		ExitGames::Common::Hashtable event;
		m_loadBalancingClient->opRaiseEvent(
			true,
			event,
			Event_SendInitDataForOtherPlayer,
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
	unsigned int SyncOnlineTwoPlayerMatchEngine::CalcCheckSum(void* pData, int size)
	{
		std::uint8_t* p = reinterpret_cast<std::uint8_t*>(pData);
		unsigned int checkSum = 0;
		for (int i = 0; i < size; i++) {
			checkSum += p[i] + i;
		}

		return checkSum;

	}
	void SyncOnlineTwoPlayerMatchEngine::SendPadData()
	{
		ONLINE_LOG("SendPadData:frameNo = %d\n", m_frameNo);
		// 送るパッドデータを構築する。
		SPadData padData;
		padData.dataType = DirectMessageType_PadData;
		padData.xInputState = g_pad[0]->GetXInputState();
		padData.frameNo = m_frameNo;
		// チェックサム用のデータを追加する。
		padData.checksum = CalcCheckSum(&padData, sizeof(SPadData) - 4);
		
		auto itFind = m_padData[0].find(m_frameNo);
		m_padData[0].insert({ m_frameNo , padData});
		m_loadBalancingClient->sendDirect(
			(std::uint8_t*)&padData,
			sizeof(padData)
		);
	}
	void SyncOnlineTwoPlayerMatchEngine::RequestResendPadData(int frameNo)
	{
		ONLINE_LOG("RequestResendPadData : frameNo = %d\n", frameNo);
		SRequestResendPadData reqResendPadData;
		reqResendPadData.dataType = DirectMessageType_RequestResendPadData;
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
		switch (m_state) {
		case State::INITIALIZED: {
			ExitGames::LoadBalancing::ConnectOptions connectOption;
			connectOption.setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS())).setUsername(PLAYER_NAME + GETTIMEMS());
			connectOption.setTryUseDatagramEncryption(true);
			m_loadBalancingClient->connect(connectOption);
			m_state = State::CONNECTING;
		}break;
		case State::CONNECTED: {
			// サーバーに接続できたので、部屋を作る。
			ExitGames::LoadBalancing::RoomOptions roomOption;
			// 部屋の最大人数は二人
			roomOption.setMaxPlayers(2);
			// sendDirect()関数(P2P)の動作は誰とでも自由に通信できるモード。
			roomOption.setDirectMode(ExitGames::LoadBalancing::DirectMode::ALL_TO_ALL);
			m_loadBalancingClient->opJoinRandomOrCreateRoom(
				ExitGames::Common::JString(),
				roomOption,
				ExitGames::Common::Hashtable(),
				2
			);
			//m_loadBalancingClient->opJoinOrCreateRoom(L"Test", roomOption);
			m_state = State::JOINING;
		}break;
		case State::JOINED:
			if (m_otherPlayerState == OtherPlayerState_JoinedRoom) {
				// すべてのプレイヤーがルームにそろった。
				// プレイヤーを初期化するための情報を送る。
				SendInitDataOtherPlayer();
				
				m_timer = 0.0f;
				// 他プレイヤーの初期化情報受け取り待ち。
				m_state = State::WAIT_RECV_INIT_DATA_OTHER_PLAYER;
			}
			break;
		case State::WAIT_START_GAME:
			m_timer += g_gameTime->GetFrameDeltaTime();
			if (m_timer > 1.0f) {
				// 相手方にこのデータが届いておらず、ずっとここで待つことがあったので、
				// 1秒ごとにプレイヤーを初期化するためのデータを再送する。
				SendInitDataOtherPlayer();
				m_timer = 0.0f;
			}
			if (m_otherPlayerState == OtherPlayerState_PossibleGameStart
				&& m_isPossibleGameStart) {
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
				if (m_otherPlayerState == OtherPlayerState_LeftRoom) {
					// 他プレイヤーが部屋から抜けた。
					m_errorFunc();
					m_loadBalancingClient->disconnect();
					break;
				}
				auto it = m_padData[1].find(m_playFrameNo);
				if (it != m_padData[1].end()) {
#ifdef ENABLE_ONLINE_PAD_LOG
					char text[256];
					if (m_playerType == PlayerType_Host) {
						if (m_padData[0][m_playFrameNo].xInputState.Gamepad.sThumbLX == 0) {
							printf("hoge");
						}
						// ホストは送信データを出力する。
						sprintf(text, "frameNo = %d, %x, %x\n",
							m_playFrameNo,
							(int)m_padData[0][m_playFrameNo].xInputState.Gamepad.sThumbLX,
							(int)m_padData[0][m_playFrameNo].xInputState.Gamepad.sThumbLY);
					}
					else {
						// クライアントは受信データを出力する。
						sprintf(text, "frameNo = %d, %x, %x\n",
							m_playFrameNo,
							(int)m_padData[1][m_playFrameNo].xInputState.Gamepad.sThumbLX,
							(int)m_padData[1][m_playFrameNo].xInputState.Gamepad.sThumbLY);
					}

					fwrite(text, strlen(text), 1, fp);
#endif
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
						m_errorFunc(); 
						m_loadBalancingClient->disconnect();
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
		case State::DISCONNECTED:
			m_state = State::INITIALIZED;
			break;
		default:
			break;
		}

		m_loadBalancingClient->service();
	}
	
	

	void SyncOnlineTwoPlayerMatchEngine::leaveRoomEventAction(int playerNr, bool isInactive)
	{
		// 部屋からプレイヤーが抜けたので、ゲーム終了。
		m_otherPlayerState = OtherPlayerState_LeftRoom;
	}
	
	void SyncOnlineTwoPlayerMatchEngine::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
	{
		ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
		switch (eventCode) {
		case Event_SendInitDataForOtherPlayer:
			if (m_state == WAIT_RECV_INIT_DATA_OTHER_PLAYER) {
				m_allPlayerJoinedRoomFunc(m_recieveDataOnGameStart.get(), m_recieveDataSize);
				m_state = WAIT_START_GAME;
			}
			break;
		case Event_PossibleGameStartOtherPlayer:
			m_otherPlayerState = OtherPlayerState_PossibleGameStart;
			break;
		}
	}
	void SyncOnlineTwoPlayerMatchEngine::onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay)
	{
		auto valueObj = (ExitGames::Common::ValueObject<std::uint8_t*>*) & msg;
		const int* sizes = valueObj->getSizes();
		std::uint8_t* pData = (std::uint8_t*)valueObj->getDataCopy();
		int dataType = (int)(*pData);
		switch (dataType) {
		case DirectMessageType_PadData: {
			// パッド情報
			SPadData padData;
			memcpy(&padData, pData, sizes[0]);
			// チェックサムを利用した誤り検出を行う。
			// 送られてきたデータのチェックサム用のデータを計算。
			unsigned int checksum = CalcCheckSum(&padData, sizeof(padData)-4);
			// 計算した値と送られてきた値が同じか調べる。
			if (checksum == padData.checksum) {
				// チェックサム通過。
				// 誤りは起きていない可能性が高い。
				auto it = m_padData[1].find(padData.frameNo);
				if (it == m_padData[1].end()) {
					// 
					m_padData[1].insert({ padData.frameNo , padData });
				}
			}
		}break;
		case DirectMessageType_RequestResendPadData: {
			// パッドデータの再送リクエストを受けたので、過去のパッドデータを再送する。
			SRequestResendPadData reqResendPadData;
			memcpy(&reqResendPadData, pData, sizes[0]);

			auto it = m_padData[0].find(reqResendPadData.frameNo);
			if (it != m_padData[0].end()) {
				// パッドデータができている。
				m_loadBalancingClient->sendDirect(
					(std::uint8_t*)&m_padData[0][reqResendPadData.frameNo],
					sizeof(m_padData[0][reqResendPadData.frameNo])
				);
			}
		}break;
		}

	}
	// callbacks for operations on PhotonLoadBalancing server
	void SyncOnlineTwoPlayerMatchEngine::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
	{
		if (errorCode)
		{
			// サーバーへの接続エラーが発生したので、切断済みにする。
			ONLINE_LOG_W(errorString.toString());
			m_state = State::DISCONNECTED;
			return;
		}
		// 部屋に入れた。
		m_state = State::CONNECTED;
	}
	void SyncOnlineTwoPlayerMatchEngine::disconnectReturn(void)
	{
		// 切断済み
		m_state = State::DISCONNECTED;
	}
	void SyncOnlineTwoPlayerMatchEngine::connectionErrorReturn(int errorCode)
	{
		// 接続に失敗したので、切断済みにする。
		m_state = State::DISCONNECTED;
	}
	void SyncOnlineTwoPlayerMatchEngine::joinRandomOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString)
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
			m_otherPlayerState = OtherPlayerState_JoinedRoom;
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
			m_otherPlayerState = OtherPlayerState_JoinedRoom;
		}
	}
}