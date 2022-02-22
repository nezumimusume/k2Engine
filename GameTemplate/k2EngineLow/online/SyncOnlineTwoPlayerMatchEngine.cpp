#include "k2EngineLowPreCompile.h"
#include "online/SyncOnlineTwoPlayerMatchEngine.h"
#include <random>


namespace nsK2EngineLow {
	namespace
	{
		const ExitGames::Common::JString PLAYER_NAME = L"user";
		const float MAX_FPS = 30.0f;	// オンライン対戦時の最大FPS
	}

	SyncOnlineTwoPlayerMatchEngine::~SyncOnlineTwoPlayerMatchEngine()
	{
		g_engine->SetFrameRateMode(
			m_frameRateInfoBackup.frameRateMode,
			m_frameRateInfoBackup.maxFPS
		);

		m_loadBalancingClient->opLeaveRoom();
		m_loadBalancingClient->opLeaveLobby();
		m_loadBalancingClient->disconnect();
//#ifdef ENABLE_ONLINE_PAD_LOG
//		fclose(m_fpLog);
//#endif
	}

	//オンライン通信エンジンの初期化関数
	void SyncOnlineTwoPlayerMatchEngine::Init(
		const wchar_t* appID,			//フォトンのアプリケーションID
		const wchar_t* appVersion,		//アプリケーションバージョン
		void* pSendData,				//送るデータ
		int sendDataSize,				//送るデータのサイズ
		//すべてのプレイヤーがルームにジョインしたときに呼ばれる処理。
		std::function<void(void* pRecvData, int dataSize)> onAllPlayerJoinedRoom,
		//すべてのプレイヤーがゲームスタートした時に呼ばれる処理。
		std::function<void()> onAllPlayerPossibleGameStart,
		//エラーの時に呼ばれる処理。
		std::function<void()> onError		)
	{
//#ifdef ENABLE_ONLINE_PAD_LOG
//		m_fpLog = fopen("log.txt", "w");
//#endif
		m_allPlayerNotifyPossibleGameStartFunc = onAllPlayerPossibleGameStart;
		m_allPlayerJoinedRoomFunc = onAllPlayerJoinedRoom;
		m_errorFunc = onError;
		ExitGames::LoadBalancing::Client* p = new ExitGames::LoadBalancing::Client(
			*this,
			appID,			//フォトンのアプリケーションID
			appVersion,		//フォトンのバージョン
			ExitGames::Photon::ConnectionProtocol::TCP,
			true
		);
		m_loadBalancingClient.reset(p);
		m_loadBalancingClient->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
		ExitGames::Common::Base::setListener(this);
		ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
		//ゲーム開始時に転送するデータを設定する
		m_sendDataOnGameStart = std::make_unique<std::uint8_t[]>(sendDataSize);
		memcpy(m_sendDataOnGameStart.get(), pSendData, sendDataSize);
		//ゲーム開始時に転送するデータのサイズを設定する
		m_sendDataSizeOnGameStart = sendDataSize;
		//初期化済みにする
		m_isInited = true;

		// 固定フレームの最大FPS30に設定する。
		m_frameRateInfoBackup = g_engine->GetFrameRateInfo();
		//フレームレートモードを設定する。
		g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, MAX_FPS);
	}

	//他プレイヤーを初期化するための情報を送る関数
	void SyncOnlineTwoPlayerMatchEngine::SendInitDataOtherPlayer()
	{
		// ルームにジョインしたことを通知。
		ExitGames::LoadBalancing::RaiseEventOptions eventOpt;
		ExitGames::Common::Hashtable event;

		event.put(0, m_sendDataOnGameStart.get(), m_sendDataSizeOnGameStart);

		m_loadBalancingClient->opRaiseEvent(
			true,
			event,
			enEvent_SendInitDataForOtherPlayer,
			eventOpt
		);
	}

	//ゲーム開始可能になったことを他プレイヤーに通知する関数
	void SyncOnlineTwoPlayerMatchEngine::SendPossibleGameStart()
	{
		// ゲーム開始ベントを送信。
		ExitGames::LoadBalancing::RaiseEventOptions eventOpt;

		ExitGames::Common::Hashtable event;

		m_loadBalancingClient->opRaiseEvent(
			true,
			event,
			enEvent_PossibleGameStartOtherPlayer,
			eventOpt
		);
	}

	//チェックサムを計算する関数
	unsigned int SyncOnlineTwoPlayerMatchEngine::CalcCheckSum(void* pData, int size)
	{
		std::uint8_t* p = reinterpret_cast<std::uint8_t*>(pData);
		unsigned int checkSum = 0;
		for (int i = 0; i < size; i++) {
			checkSum += p[i] + i;
		}

		return checkSum;
	}

	//パッド情報をP2Pで直接送信する関数
	void SyncOnlineTwoPlayerMatchEngine::SendPadDataDirect()
	{
		ONLINE_LOG("SendPadData:frameNo = %d\n", m_frameNo);

		// 送るパッドデータを構築する。
		SPadData padData;
		//パッドデータの種類を設定する
		padData.dataType = enDirectMessageType_PadData;
		//パッドデータのXInputステートを設定する
		padData.xInputState = g_pad[0]->GetXInputState();
		//パッドデータのフレーム番号を設定する
		padData.frameNo = m_frameNo;
		// チェックサム用のデータを追加する。
		padData.checksum = CalcCheckSum(&padData, sizeof(SPadData) - 4);

		//プレイヤー番号を設定
		int plNo = GetPlayerNo();
		auto itFind = m_padData[plNo].find(m_frameNo);
		m_padData[plNo].insert({ m_frameNo , padData});
		m_loadBalancingClient->sendDirect(
			(std::uint8_t*)&padData,
			sizeof(padData)
		);
	}

	//パッドデータの再送リクエストをP2Pで送信する関数
	void SyncOnlineTwoPlayerMatchEngine::SendRequestResendPadDataDirect(int frameNo)
	{
		ONLINE_LOG("RequestResendPadData : frameNo = %d\n", frameNo);

		SRequestResendPadData reqResendPadData;
		reqResendPadData.dataType = enDirectMessageType_RequestResendPadData;
		reqResendPadData.frameNo = frameNo;

		m_loadBalancingClient->sendDirect(
			(std::uint8_t*)&reqResendPadData,
			sizeof(reqResendPadData)
		);
	}

	//初期化ステップの更新処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update_Initialized()
	{
		ONLINE_LOG("Update_Initialized()\n");

		ExitGames::LoadBalancing::ConnectOptions connectOption;
		connectOption.setAuthenticationValues(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS())).setUsername(PLAYER_NAME + GETTIMEMS());
		connectOption.setTryUseDatagramEncryption(true);
		m_loadBalancingClient->connect(connectOption);
		//状態を「サーバー接続中」に設定する
		m_state = State::CONNECTING;
	}

	//Photonサーバーに接続済みの時の更新処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update_Connected()
	{
		ONLINE_LOG("Update_Connected()\n");

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
		//タイマーを初期化
		m_timer = 0.0f;
		std::random_device rnd;
		//待ちの最大秒数を設定する
		m_waitLimitTime = 30;
		//状態を「ルームにジョイン中」に設定する
		m_state = State::JOINING;
	}

	//部屋に入っているときの処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update_Joined()
	{
		ONLINE_LOG("Update_Joined()\n");

		//１フレームの経過時間をタイマーに加算する
		m_timer += g_gameTime->GetFrameDeltaTime();
		//タイマーが待ちの最大秒数を超えたとき、
		if (m_timer > m_waitLimitTime) {
			// 10秒+α秒経過したので、一旦サーバーから切断して、再接続。
			m_state = State::DISCONNECTING;
			m_loadBalancingClient->disconnect();
		}
		//相手プレイヤーが部屋に入ってきたとき、
		if (m_otherPlayerState == enOtherPlayerState_JoinedRoom) {
			// すべてのプレイヤーがルームにそろった。
			// プレイヤーを初期化するための情報を送る。
			SendInitDataOtherPlayer();
			//タイマーを初期化する
			m_timer = 0.0f;
			//待ちの最大秒数を設定する
			m_waitLimitTime =30;
			// 他プレイヤーの初期化情報受け取り待ちへ遷移する。
			m_state = State::WAIT_RECV_INIT_DATA_OTHER_PLAYER;
		}
	}

	//ゲームの開始待ち中の更新処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update_WaitStartGame()
	{
		ONLINE_LOG("Update_WaitStartGame()\n");

		//１フレームの経過時間をタイマーに加算する
		m_timer += g_gameTime->GetFrameDeltaTime();
		if (m_timer > 1.0f) {
			// 相手方にこのデータが届いておらず、ずっとここで待つことがあったので、
			// 1秒ごとにプレイヤーを初期化するためのデータを再送する。
			SendInitDataOtherPlayer();
			//タイマーを初期化する
			m_timer = 0.0f;
		}
		//タイマーが待ちの最大秒数を超えた時、
		if (m_timer > m_waitLimitTime) {
			// 10秒+α秒待ってもパケットが届かなかったので、一旦切断して、再接続。
			m_state = State::DISCONNECTING;
			m_loadBalancingClient->disconnect();
		}
		//相手プレイヤーの状態がゲーム開始可能な状態で、
		if (m_otherPlayerState == enOtherPlayerState_PossibleGameStart
			&& m_isPossibleGameStart) {
			//すべてのプレイヤーがゲーム開始可能であることを通知
			m_allPlayerNotifyPossibleGameStartFunc();
			//状態をパッドデータのバッファリング中に設定
			m_state = State::IN_GAME_BUFFERING_PAD_DATA;
		}
	}

	//インゲーム(パッドデータのバッファリング中)
	void SyncOnlineTwoPlayerMatchEngine::Update_InGameBufferingPadData()
	{
		ONLINE_LOG("Update_InGameBufferingPadData()\n");

		// パッドデータを送信。
		if (m_frameNo < 3) {
			// 3フレーム分だけ送る。
			SendPadDataDirect();
			//パッドのフレーム番号を次に進める
			m_frameNo++;
		}
		else {
			// バッファリング終わり。
			m_state = State::IN_GAME;
		}
	}

	//インゲーム中の更新処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update_InGame()
	{
		ONLINE_LOG("Update_InGame()\n");

		//ループカウント変数
		int loopCount = 0;
		//自身のプレイヤー番号
		int plNo = GetPlayerNo();
		//通信相手のプレイヤー番号
		int otherPlNo = GetOtherPlayerNo();
		while(true) {
			// 他プレイヤーが部屋から抜けたとき、
			if (m_otherPlayerState == enOtherPlayerState_LeftRoom) {
				ONLINE_LOG("プレイヤーが部屋から抜けた");
				m_loadBalancingClient->disconnect();
				m_state = State::DISCONNECTED;
				break;
			}
			auto it = m_padData[otherPlNo].find(m_playFrameNo);
			if (it != m_padData[otherPlNo].end()) {
//#ifdef ENABLE_ONLINE_PAD_LOG
//				// 再生したパッドのログを出力。
//				OutputPlayPadDataLog();
//#endif
				// 再生フレームのパッド情報を受け取っている。
				m_pad[plNo].Update(m_padData[plNo][m_playFrameNo].xInputState);
				m_pad[otherPlNo].Update(it->second.xInputState);
				if (m_padData[plNo].size() > 100) {
					// サイズが100を超えたので、一番古いフレームのデータを削除する。
					int delFrameNo = INT32_MAX;
					for (auto it = m_padData[plNo].begin(); it != m_padData[plNo].end(); it++)
					{
						if (delFrameNo < it->first) {
							delFrameNo = it->first;
						}
					}
					// 一番古いデータを削除。
					m_padData[plNo].erase(delFrameNo);
					m_padData[otherPlNo].erase(delFrameNo);
				}

				break;
			}
			else {
				// データが来ていない or パケットロスト
				// 再送リクエストを送る。
				SendRequestResendPadDataDirect(m_playFrameNo);
				//ループカウント変数のカウントを進める
				loopCount++;
				//100フレーム遅延させる
				Sleep(100);
				m_loadBalancingClient->service();
				//ループカウント変数のカウンターが100になったら、
				 if (loopCount == 100) {
					OutputDebugStringA("ループカウント変数のカウンターが100になったら");
					// 接続エラー。
					m_loadBalancingClient->disconnect();
					m_state = State::DISCONNECTED;
					break;
				}
			}
		}
		//パッド情報をP2Pで直接送信する
		SendPadDataDirect();
		//現在のゲーム進行フレーム番号を次に進める
		m_playFrameNo++;
		//現在のパッドのフレーム番号を次に進める
		m_frameNo++;
	}

	//ロビーの更新処理関数
	void SyncOnlineTwoPlayerMatchEngine::Update()
	{
		//初期化済みじゃないときは処理を行わない
		if (!m_isInited) {
			return;
		}
		//初期化済みのとき以下を実行する
		//状態によって処理を分岐。
		switch (m_state) {
		//初期化状態の時、
		case State::INITIALIZED:
			Update_Initialized();
			break;
		//サーバーに接続済み状態の時、
		case State::CONNECTED:
			Update_Connected();
			break;
		//ルームにジョイン済み状態の時、
		case State::JOINED:
			Update_Joined();
			break;
			//ゲームの開始待ち状態の時、
		case State::WAIT_START_GAME:
			Update_WaitStartGame();
			break;
		//パッドデータのバッファリング中状態の時、
		case State::IN_GAME_BUFFERING_PAD_DATA:
			Update_InGameBufferingPadData();
			break;
		//ゲームプレイ中状態の時、
		case State::IN_GAME:
			Update_InGame();
			break;
		//サーバーから切断済み状態の時、
		case State::DISCONNECTED:
			m_errorFunc();
			m_errorFunc = nullptr;
			m_state = State::END;
			break;
		//それ以外の時、
		default:
			break;
		}

		m_loadBalancingClient->service();
	}

	//ユーザーが部屋から抜けたときに呼ばれる関数
	void SyncOnlineTwoPlayerMatchEngine::leaveRoomEventAction(int playerNr, bool isInactive)
	{
		// 部屋からプレイヤーが抜けたので、ゲーム終了。
		m_otherPlayerState = enOtherPlayerState_LeftRoom;

		OutputDebugStringA("相手がルームから退室\n");
	}

	//opRaiseEvent()関数で送られたメッセージを受信した場合に、呼び出されるコールバック関数
	void SyncOnlineTwoPlayerMatchEngine::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
	{
		auto eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
		switch (eventCode) {
		case enEvent_SendInitDataForOtherPlayer:
			if (m_state == WAIT_RECV_INIT_DATA_OTHER_PLAYER) {
				//K2_ASSERT(!m_isHoge, "二回呼ばれている");
				m_isHoge = true;

				ONLINE_LOG("enEvent_SendInitDataForOtherPlayer\n");

				auto valuObj = (ExitGames::Common::ValueObject<std::uint8_t*>*)(eventContent.getValue(0));
				m_recieveDataSize = valuObj->getSizes()[0];
				m_recieveDataOnGameStart = std::make_unique<std::uint8_t[]>(m_recieveDataSize);
				auto pSrcData = valuObj->getDataCopy();
				memcpy(m_recieveDataOnGameStart.get(), pSrcData, m_recieveDataSize);
				m_allPlayerJoinedRoomFunc(m_recieveDataOnGameStart.get(), m_recieveDataSize);
				m_state = WAIT_START_GAME;
			}
			break;
		case enEvent_PossibleGameStartOtherPlayer:
			m_otherPlayerState = enOtherPlayerState_PossibleGameStart;
			break;
		}
	}

	//P2P通信でenDirectMessageType_PadDataのメッセージが送られてきたときの処理関数
	void SyncOnlineTwoPlayerMatchEngine::OnDirectMessageType_PadData(std::uint8_t* pData, int size)
	{
		// パッド情報
		SPadData padData;
		memcpy(&padData, pData, size);
		// チェックサムを利用した誤り検出を行う。
		// 送られてきたデータのチェックサム用のデータを計算。
		unsigned int checksum = CalcCheckSum(&padData, sizeof(padData) - 4);

		int otherPlNo = GetOtherPlayerNo();
		// 計算した値と送られてきた値が同じか調べる。
		if (checksum == padData.checksum) {
			// チェックサム通過。
			// 誤りは起きていない可能性が高い。
			auto it = m_padData[otherPlNo].find(padData.frameNo);
			if (it == m_padData[otherPlNo].end()) {
				//
				m_padData[otherPlNo].insert({ padData.frameNo , padData });
			}
		}
	}

	//P2P通信でenDirectMessageType_RequestResendPadDataのメッセージが送られてきたときの処理関数
	void SyncOnlineTwoPlayerMatchEngine::OnDirectMessageType_RequestResendPadData(std::uint8_t* pData, int size)
	{
		// パッドデータの再送リクエストを受けたので、過去のパッドデータを再送する。
		SRequestResendPadData reqResendPadData;
		memcpy(&reqResendPadData, pData, size);

		int plNo = GetPlayerNo();
		auto it = m_padData[plNo].find(reqResendPadData.frameNo);
		if (it != m_padData[plNo].end()) {
			// パッドデータができている。
			m_loadBalancingClient->sendDirect(
				(std::uint8_t*)&m_padData[plNo][reqResendPadData.frameNo],
				sizeof(m_padData[plNo][reqResendPadData.frameNo])
			);
		}
	}

	void SyncOnlineTwoPlayerMatchEngine::disconnectReturn(void)
	{
		if (m_state != State::END 
			&& m_state != State::IN_GAME
		) {
			// 終了状態の際に再度切断済みに行かないようにする。
			// 切断済みにする。
			ONLINE_LOG("disconnectReturn\n");
			m_state = State::DISCONNECTED;
		}
	}

	//sendDirect()関数(P2P通信)で送られたメッセージを受信した場合に、呼び出されるコールバック関数
	void SyncOnlineTwoPlayerMatchEngine::onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay)
	{
		// 送られてきたデータをコピー。
		auto valueObj = (ExitGames::Common::ValueObject<std::uint8_t*>*) & msg;
		const int* sizes = valueObj->getSizes();
		std::uint8_t* pData = (std::uint8_t*)valueObj->getDataCopy();
		// データの先頭にデータタイプがつけられている。
		int dataType = (int)(*pData);

		// データタイプに応じて処理を分岐。
		switch (dataType) {
		case enDirectMessageType_PadData:
			OnDirectMessageType_PadData(pData, sizes[0]);
			break;
		case enDirectMessageType_RequestResendPadData:
			OnDirectMessageType_RequestResendPadData(pData, sizes[0]);
			break;
		}
	}

	// callbacks for operations on PhotonLoadBalancing server
	void SyncOnlineTwoPlayerMatchEngine::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
	{
		if (errorCode)
		{
			// サーバーへの接続エラーが発生したので、切断済みにする。

			//ONLINE_LOG_W(errorString.toString());
			ONLINE_LOG("connectReturn\n");
			m_state = State::DISCONNECTED;
			return;
		}
		// 部屋に入れた。
		m_state = State::CONNECTED;
	}

	void SyncOnlineTwoPlayerMatchEngine::connectionErrorReturn(int errorCode)
	{
		// 接続に失敗したので、切断済みにする。
		ONLINE_LOG("connectionErrorReturn errorCode = %d\n", errorCode);
		m_state = State::DISCONNECTED;
	}

	//ルームを作成 or 入室したときに呼ばれる処理関数
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
			m_playerType = enPlayerType_Host;

			OutputDebugStringA("１番目にルームに入った。\n");
		}
		else {
			// クライアントなので、すでにホストはいるはずなので、
			// 他プレイヤーは部屋にジョイン済みにする。
			m_otherPlayerState = enOtherPlayerState_JoinedRoom;
			m_playerType = enPlayerType_Client;

			OutputDebugStringA("２番目にルームに入った。\n");
		}
		// ルームに入った。
		m_state = State::JOINED;
	}


	void SyncOnlineTwoPlayerMatchEngine::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
	{
		if (m_playerType == enPlayerType_Host
			&& playerNr == 2
		) {
			// クライアントがジョインしてきたので、他プレイヤーをジョイン済みにする。
			m_otherPlayerState = enOtherPlayerState_JoinedRoom;
		}
	}
#ifdef ENABLE_ONLINE_PAD_LOG

	void SyncOnlineTwoPlayerMatchEngine::OutputPlayPadDataLog()
	{
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
	}
#endif
}