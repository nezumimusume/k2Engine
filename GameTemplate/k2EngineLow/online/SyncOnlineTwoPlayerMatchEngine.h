#pragma once

#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Client.h"
#include <functional>

namespace nsK2EngineLow {
	/// <summary>
	/// 完全同期二人対戦オンラインマッチの機能を提供するエンジン。
	/// </summary>
	/// <remark>
	/// PhotonCloudSDKを利用したロビークラス。
	/// 本クラスを利用するためには、PhotonCloudにてユーザーアカウントを作成して、
	/// 作成するアプリを登録する必要があります。
	/// ユーザーアカウントの作成、アプリの登録がまだの場合は、下記URLにジャンプして
	/// 登録を行ってください。
	/// https://www.photonengine.com/ja/
	/// </remark>
	class SyncOnlineTwoPlayerMatchEngine : ExitGames::LoadBalancing::Listener, Noncopyable{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~SyncOnlineTwoPlayerMatchEngine();
		/// <summary>
		/// 初期化。
		/// </summary>
		
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="appID">アプリケーションID</param>
		/// <param name="appVersion">アプリケーションバージョン</param>
		/// <param name="pSendData">マッチング時に相手に渡すゲーム固有のデータ。使用するキャラクターの番号など。</param>
		/// <param name="sendDataSize">マッチング時に相手に渡すデータのサイズ</param>
		/// <param name="onAllPlayerJoinedRoom">すべてのプレイヤーがルームに参加した時に呼ばれる処理。</param>
		/// <param name="onAllPlayerPossibleGameStart">すべてのプレイヤーがゲーム開始可能になった時に呼ばれる処理。</param>
		/// <param name="onError">エラー発生時に呼ばれる関数</param>
		void Init(
			const wchar_t* appID,
			const wchar_t* appVersion,
			void* pSendData,
			int sendDataSize,
			std::function<void(void* pRecvData, int dataSize)> onAllPlayerJoinedRoom,
			std::function<void()> onAllPlayerPossibleGameStart,
			std::function<void()> onError
		);
		/// <summary>
		/// ロビーの更新処理
		/// </summary>
		void Update();
		/// <summary>
		/// ゲームパッドの取得。
		/// </summary>
		/// <param name="no"></param>
		/// <returns></returns>
		GamePad& GetGamePad(int no)
		{
			return m_pad[no];
		}
		/// <summary>
		/// ゲーム開始可能になったことを通知。
		/// </summary>
		void NotifyPossibleStartPlayGame()
		{
			m_isPossibleGameStart = true;
			SendPossibleGameStart();
		}
	private:
		/// <summary>
		/// ユーザーが部屋から抜けたときに呼ばれる処理。
		/// </summary>
		/// <param name="playerNr"></param>
		/// <param name="isInactive"></param>
		void leaveRoomEventAction(int playerNr, bool isInactive) override;
		/// <summary>
		/// ユーザー定義のイベントを受け取ったときに呼ばれる処理。
		/// </summary>
		/// <param name="playerNr"></param>
		/// <param name="eventCode"></param>
		/// <param name="eventContent"></param>
		void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;
		/// <summary>
		/// photonサーバーへの接続リクエストを実行した場合に呼び出されるコールバック関数。
		/// </summary>
		/// <param name="errorCode">エラーコード</param>
		/// <param name="errorString">エラー文字列</param>
		/// <param name="region">リージョン</param>
		/// <param name="cluster">クラスター</param>
		void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
		/// <summary>
		/// sendDirectで送られメッセージを受信した場合に呼び出されるコールバック関数。
		/// </summary>
		/// <param name="msg"></param>
		/// <param name="remoteID"></param>
		/// <param name="relay"></param>
		void onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay) override;

		void disconnectReturn(void) override;
		/// <summary>
		/// ルームを作成 or 入室したときに呼ばれる処理。
		/// </summary>
		/// <param name="localPlayerNr"></param>
		/// <param name="gameProperties"></param>
		/// <param name="playerProperties"></param>
		/// <param name="errorCode"></param>
		/// <param name="errorString"></param>
		void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void connectionErrorReturn(int errorCode) override;
		void clientErrorReturn(int errorCode) override {}
		void warningReturn(int warningCode) override {}
		void serverErrorReturn(int errorCode) override {}
		void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override {}
		void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;
		void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override {}
	private:
		/// <summary>
		/// ゲーム開始可能になったことを他プレイヤーに通知。
		/// </summary>
		void SendPossibleGameStart();
		/// <summary>
		/// ルームに入ったことを他プレイヤーに通知。
		/// </summary>
		void SendJoined();
		/// <summary>
		/// パッド情報を送信。
		/// </summary>
		void SendPadData();
		/// <summary>
		/// サーバーから切断。
		/// </summary>
		void Disconnect();
		/// <summary>
		/// パッドデータの再送リクエスト
		/// </summary>
		/// <param name="frameNo">再送リクエストを行うフレーム番号</param>
		void RequestResendPadData( int frameNo );
	private:

		enum State
		{
			INITIALIZED = 0,		// 初期化
			CONNECTING,				// サーバー接続中
			CONNECTED,				// サーバーに接続済み
			JOINING,				// ルームにジョイン中
			JOINED,					// ルームにジョイン済み。
			WAIT_RECV_START_DATA,	// 開始データの受け取り待ち。
			WAIT_START_GAME,		// ゲームの開始待ち
			IN_GAME_BUFFERING_PAD_DATA,	// パッドデータのバッファリング中
			IN_GAME,				// ゲームプレイ中。
			LEAVING,				// ルームから退出中。
			LEFT,					// ルームから退出。
			DISCONNECTING,			// サーバーから切断中。
			DISCONNECTED			// サーバーから切断済み。
		};
		/// <summary>
		/// イベント
		/// </summary>
		enum Event {
			Event_JoinedOtherPalyer,			// 他プレイヤーがルームにジョインした。
			Event_PossibleGameStartOtherPlayer, // 他プレイヤーがゲーム開始可能になったことを通知・
		};
		/// <summary>
		/// パッドデータ
		/// </summary>
		struct SPadData {
			int dataType;				// データの種類。
			XINPUT_STATE xInputState;	// XInputステート
			int frameNo;				// フレーム番号
		};
		/// <summary>
		/// パッドデータの再送リクエスト
		/// </summary>
		struct SRequestResendPadData {
			int dataType;
			int frameNo;
		};
		/// <summary>
		/// プレイヤータイプ
		/// </summary>
		enum PlayerType {
			PlayerType_Host,		// ホスト
			PlayerType_Client,		// クライアント
			PlayerType_Undef,		// 
		};
		using OnAllPlayerJoinedRoom = std::function<void(void* pRecvData, int dataSize)>;
		using OnErrorFunc = std::function<void()>;
		using OnAllPlayerNotifyPossibleGameStart = std::function<void()>;
		std::unique_ptr<ExitGames::LoadBalancing::Client> m_loadBalancingClient;	// ロードバランサ―。
		ExitGames::Common::Logger m_logger;		// ログ出力処理。
		State m_state = INITIALIZED;			// 状態。
		int m_frameNo = 0;						// フレーム番号。
		int m_playFrameNo = 0;
		bool m_isJoinedOtherPlayer = false;		// 他プレイヤーがルームにジョインした。
		bool m_isPossibleGameStartOtherPlayer = false;	// 他プレイヤーもゲーム開始リクエストを受けた？
		bool m_isPossibleGameStart = false;		//ゲーム開始可能
		std::map< int, SPadData> m_padData[2];	// パッドデータ。	
		GamePad m_pad[2];						// ゲームパッド。
		OnAllPlayerJoinedRoom m_allPlayerJoinedRoomFunc = nullptr;	// すべてのプレイヤーがルームに参加した。
		OnAllPlayerNotifyPossibleGameStart m_allPlayerNotifyPossibleGameStartFunc = nullptr;	// すべてのプレイヤーがゲーム開始可能であることを通知した。
		OnErrorFunc m_errorFunc = nullptr;							// 通信エラーが起きた時に呼ばれる関数。
		std::unique_ptr<std::uint8_t[]> m_recieveDataOnGameStart;	// ゲーム開始のために受け取ったデータ。
		int m_recieveDataSize = 0;									// ゲーム開始のために受け取ったデータのサイズ。
		bool m_isInited = false;									// 初期化済み？
		float m_timer = 0.0f;										// タイマー
		PlayerType m_playerType = PlayerType_Undef;
	};
}