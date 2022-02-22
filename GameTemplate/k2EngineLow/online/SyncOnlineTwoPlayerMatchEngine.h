#pragma once

#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Client.h"
#include <functional>

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
	/// <summary>
	/// 完全同期二人対戦オンラインマッチの機能を提供するエンジン。
	/// </summary>
	/// <remark>
	/// 本クラスを利用するためには、PhotonCloudにてユーザーアカウントを作成して、
	/// 作成するアプリを登録する必要があります。
	/// ユーザーアカウントの作成、アプリの登録がまだの場合は、下記URLにジャンプして
	/// 登録を行ってください。
	/// https://www.photonengine.com/ja/
	/// このエンジンを利用すると完全に同期がとれた、２人対戦用のパッド情報を取得することができます。
	/// このパッド情報を使って、２人対戦の処理を記述することで、オンライン対戦のゲームを作成することができます。
	/// 以下、本エンジンの処理のフローとなります。
	/// 1. Init()関数を使用して、初期化を行う。
	/// 2. 毎フレームUpdate()関数を呼び出す。
	///		以下、Updateの処理を記述
	///		2.1 Photonクラウドサーバーへの接続
	///		2.2 ルームが存在していたら、ルームにジョイン、なければ新規作成
	///		2.3 ルームにユーザーが２人揃ったら、キャラの初期化に必要な情報を他プレイヤーに送信
	///		2.4 初期化情報が届いたら、ゲーム側に通知。Init()関数で指定されたonAllPlayerJoinedRoom()関数が呼ばれます。
	///			2.4.1 ゲーム側はゲームを始めるための初期化処理を行い、初期化完了後にNotifyPossibleStartPlayGame()関数を
	///				　呼び出して、ゲーム開始可能になったことをエンジンに通知
	///		2.5 全てのプレイヤーがゲーム可能になるまでポーリング
	///			2.5.1 全てのプレイヤーがゲーム開始可能になると、Init()関数で指定されたonAllPlayerPossibleGameStart()関数を呼び出します。
	///		2.5 パッド情報を3フレーム分バッファリング
	///		2.6 バッファリング完了後、ゲームパッドの更新を毎フレーム行う。
	///
	/// 本クラスの利用のサンプルとしてSample_24が提供されています。
	/// </remark>
	class SyncOnlineTwoPlayerMatchEngine : ExitGames::LoadBalancing::Listener{
	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~SyncOnlineTwoPlayerMatchEngine();

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
		/// <summary>
		/// プレイヤー番号を取得。
		/// ホストなら0、クライアントなら1を返します。
		/// </summary>
		/// <returns></returns>
		int GetPlayerNo() const
		{
			//ホスト
			if (m_playerType == enPlayerType_Host) {
				return 0;
			}
			// クライアント
			else
			{
				return 1;
			}
		}
		/// <summary>
		/// 対戦相手のプレイヤー番号を取得。
		/// ホストなら1、クライアントなら1を返します。
		/// </summary>
		/// <returns></returns>
		int GetOtherPlayerNo() const
		{
			// 対戦相手のプレイヤー番号は、自分の番号の反対。
			return !GetPlayerNo();
		}
	private:
		/// <summary>
		/// 初期化ステップの更新処理。
		/// Photonサーバーへの接続リクエストを送信します。
		/// </summary>
		void Update_Initialized();
		/// <summary>
		/// Photonサーバーに接続済みの時の更新処理。
		/// Photonサーバーに部屋を作成するリクエストを送信します。
		/// </summary>
		void Update_Connected();
		/// <summary>
		/// 部屋に入っているときの処理。
		/// 部屋にすべてのプレイヤーが揃うと、キャラクターの初期化情報を送信します。
		/// </summary>
		void Update_Joined();
		/// <summary>
		/// ゲームの開始待ち中の更新処理。
		/// キャラクターのロードが終わるのを待ちます。
		/// ゲーム側からNotifyPossibleStartPlayGame()関数が呼ばれるまで、
		/// Updateの処理はこのステップで止まります。
		/// ゲーム側はキャラクターのロードなどが完了して、
		/// ゲームを開始できるようになったら、NotifyPossibleStartPlayGame()関数を呼び出してください。
		/// </summary>
		void Update_WaitStartGame();
		/// <summary>
		/// インゲーム(パッドデータのバッファリング中)
		/// </summary>
		void Update_InGameBufferingPadData();
		/// <summary>
		/// インゲーム中の更新処理。
		/// パッドデータから該当するフレームのパッド情報を取得して、
		/// ゲームパッドの情報を更新します。
		/// 対戦プレイヤーの該当フレームのパッド情報が、まだ届いていない場合は、
		/// 再送リクエストを送ります。
		/// 再送リクエストを送ってもパッド情報が来ない場合は、通信エラーが発生していると判断して、
		/// エラー処理を実行します。
		/// </summary>
		void Update_InGame();
		/// <summary>
		/// P2P通信でenDirectMessageType_PadDataのメッセージが送られてきたときの処理。
		/// </summary>
		void OnDirectMessageType_PadData(std::uint8_t* pData, int size);
		/// <summary>
		/// P2P通信でenDirectMessageType_RequestResendPadDataのメッセージが送られてきたときの処理。
		/// </summary>
		/// <param name="pData"></param>
		/// <param name="size"></param>
		void OnDirectMessageType_RequestResendPadData(std::uint8_t* pData, int size);
		/// <summary>
		/// ユーザーが部屋から抜けたときに呼ばれる処理。
		/// </summary>
		/// <param name="playerNr"></param>
		/// <param name="isInactive"></param>
		void leaveRoomEventAction(int playerNr, bool isInactive) override;

		/// <summary>
		/// photonサーバーへの接続リクエストを実行した場合に呼び出されるコールバック関数。
		/// </summary>
		/// <param name="errorCode">エラーコード</param>
		/// <param name="errorString">エラー文字列</param>
		/// <param name="region">リージョン</param>
		/// <param name="cluster">クラスター</param>
		void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
		/// <summary>
		/// opRaiseEvent()関数で送られたメッセージを受信した場合に、呼び出されるコールバック関数。
		/// onRaiseEvent()関数はゲームサーバー経由でメッセージが送られます。
		/// メッセージの通信プロトコルはTCPです。
		/// </summary>
		/// <param name="playerNr"></param>
		/// <param name="eventCode"></param>
		/// <param name="eventContent"></param>
		void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;
		/// <summary>
		/// sendDirect()関数(P2P通信)で送られたメッセージを受信した場合に、呼び出されるコールバック関数。
		/// メッセージの通信プロトコルはUDPです。
		/// インゲーム中の高いレスポンスが必要な通信はsendDirect()関数を利用して、そのメッセージの受信の処理を
		/// この関数の中に記述してください。
		/// </summary>
		/// <param name="msg"></param>
		/// <param name="remoteID"></param>
		/// <param name="relay"></param>
		void onDirectMessage(const ExitGames::Common::Object& msg, int remoteID, bool relay) override;
		/// <summary>
		/// ルームを作成 or 入室したときに呼ばれる処理。
		/// </summary>
		/// <param name="localPlayerNr"></param>
		/// <param name="gameProperties"></param>
		/// <param name="playerProperties"></param>
		/// <param name="errorCode"></param>
		/// <param name="errorString"></param>
		void joinRandomOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void connectionErrorReturn(int errorCode) override;
		void clientErrorReturn(int errorCode) override {}
		void warningReturn(int warningCode) override {}
		void serverErrorReturn(int errorCode) override {}
		void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override {}
		void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;
		void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override {}
		void disconnectReturn(void) override;
	private:
		/// <summary>
		/// ゲーム開始可能になったことを他プレイヤーに通知。
		/// </summary>
		void SendPossibleGameStart();
		/// <summary>
		/// 他プレイヤーを初期化するための情報を送る。
		/// </summary>
		void SendInitDataOtherPlayer();
		/// <summary>
		/// パッド情報をP2Pで直接送信。
		/// </summary>
		void SendPadDataDirect();

		/// <summary>
		/// パッドデータの再送リクエストをP2Pで送信
		/// </summary>
		/// <param name="frameNo">再送リクエストを行うフレーム番号</param>
		void SendRequestResendPadDataDirect( int frameNo );
		/// <summary>
		/// チェックサムを計算
		/// </summary>
		/// <param name="pData">チェックサムを計算するデータの先頭アドレス</param>
		/// <param name="size">データのサイズ</param>
		unsigned int CalcCheckSum(void* pData, int size);
#ifdef ENABLE_ONLINE_PAD_LOG
		/// <summary>
		/// 再生中のパッドデータのログをファイルに出力
		/// </summary>
		void OutputPlayPadDataLog();
#endif
	private:
		/// <summary>
		/// 列挙型
		/// </summary>

		//状態
		enum State
		{
			INITIALIZED = 0,					// 初期化
			CONNECTING,							// サーバー接続中
			CONNECTED,							// サーバーに接続済み
			JOINING,							// ルームにジョイン中
			JOINED,								// ルームにジョイン済み。
			WAIT_RECV_INIT_DATA_OTHER_PLAYER,	// 開始データの受け取り待ち。
			WAIT_START_GAME,					// ゲームの開始待ち
			IN_GAME_BUFFERING_PAD_DATA,			// パッドデータのバッファリング中
			IN_GAME,							// ゲームプレイ中。
			DISCONNECTING,						// サーバーから切断中。
			DISCONNECTED,						// サーバーから切断済み。
			END,								// 終わり

		};

		//イベント
		enum EnEvent
		{
			enEvent_SendInitDataForOtherPlayer,		// 他プレイヤーの初期化情報を送る。
			enEvent_PossibleGameStartOtherPlayer	// 他プレイヤーがゲーム開始可能になったことを通知・
		};

		//sendDirect()関数で送られてくるメッセージの種類。
		enum EnDirectMessageType
		{
			enDirectMessageType_PadData,				// パッドデータ
			enDirectMessageType_RequestResendPadData,	// パッドデータの再送リクエスト
		};

		/// <summary>
		/// 構造体
		/// </summary>

		//パッドデータ
		struct SPadData
		{
			int dataType;				// データの種類。
			XINPUT_STATE xInputState;	// XInputステート
			int frameNo;				// フレーム番号
			unsigned int checksum;		// チェックサム用のデータ。
		};

		//パッドデータの再送リクエスト
		struct SRequestResendPadData
		{
			int dataType;
			int frameNo;
		};

		//プレイヤータイプ
		enum EnPlayerType
		{
			enPlayerType_Host,		// ホスト(部屋を作った)
			enPlayerType_Client,	// クライアント(既存の部屋に入った)
			enPlayerType_Undef,		// 不明
		};

		// 他プレイヤーの状態。
		enum EnOtherPlayerState
		{
			enOtherPlayerState_Undef,				// m_playerType
			enOtherPlayerState_JoinedRoom,			// 部屋に入ってきた。
			enOtherPlayerState_PossibleGameStart,	// ゲーム開始可能状態
			enOtherPlayerState_LeftRoom,			// 部屋から抜けた。
		};

		using OnAllPlayerJoinedRoom = std::function<void(void* pRecvData, int dataSize)>;
		using OnErrorFunc = std::function<void()>;
		using OnAllPlayerNotifyPossibleGameStart = std::function<void()>;
		using LoadBalancingClientPtr = std::unique_ptr<ExitGames::LoadBalancing::Client>;

		LoadBalancingClientPtr m_loadBalancingClient;						// ロードバランサ―。
		State m_state = INITIALIZED;										// 状態。
		int m_frameNo = 0;													// 現在のパッドのフレーム番号。
		int m_playFrameNo = 0;												// 現在のゲーム進行フレーム番号。
		bool m_isPossibleGameStart = false;									// ゲーム開始可能フラグ。
		std::map< int, SPadData> m_padData[2];								// パッドデータ。
		GamePad m_pad[2];													// ゲームパッド。
		OnAllPlayerJoinedRoom m_allPlayerJoinedRoomFunc = nullptr;			// すべてのプレイヤーがルームに参加した。
		OnAllPlayerNotifyPossibleGameStart m_allPlayerNotifyPossibleGameStartFunc = nullptr;	// すべてのプレイヤーがゲーム開始可能であることを通知した。
		OnErrorFunc m_errorFunc = nullptr;									// 通信エラーが起きた時に呼ばれる関数。
		std::unique_ptr<std::uint8_t[]> m_sendDataOnGameStart;				// ゲーム開始時に転送するデータ。
		int m_sendDataSizeOnGameStart;										// ゲーム開始時に転送するデータのサイズ。
		std::unique_ptr<std::uint8_t[]> m_recieveDataOnGameStart;			// ゲーム開始のために受け取ったデータ。
		int m_recieveDataSize = 0;											// ゲーム開始のために受け取ったデータのサイズ。
		bool m_isInited = false;											// 初期化済み？
		float m_timer = 0.0f;												// タイマー
		float m_waitLimitTime = 0.0f;										// 待ちの最大秒数。
		EnPlayerType m_playerType = enPlayerType_Undef;						// プレイヤーのタイプ。
		EnOtherPlayerState m_otherPlayerState = enOtherPlayerState_Undef;	// 他プレイヤーの状態。
		K2EngineLow::FrameRateInfo m_frameRateInfoBackup;								// フレームレートに関する情報のバックアップ。

		bool m_isHoge = false;
#ifdef ENABLE_ONLINE_PAD_LOG
		FILE* m_fpLog = nullptr;									// ログ出力用のファイルポインタ。
#endif
	};
}