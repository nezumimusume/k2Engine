#pragma once

#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Client.h"

namespace nsK2EngineLow {
	/// <summary>
	/// ロビーマネージャ。
	/// </summary>
	/// <remark>
	/// PhotonCloudSDKを利用したロビーマネージャークラス。
	/// 本クラスを利用するためには、PhotonCloudにてユーザーアカウントを作成して、
	/// 作成するアプリを登録する必要があります。
	/// ユーザーアカウントの作成、アプリの登録がまだの場合は、下記URLにジャンプして
	/// 登録を行ってください。
	/// https://www.photonengine.com/ja/
	/// </remark>
	class LobbyManager : ExitGames::LoadBalancing::Listener, Noncopyable{
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <remark>
		/// アプリケーションID、アプリケーションバージョンはPhoto
		/// </remark>
		/// <param name="appID">アプリケーションID</param>
		/// <param name="appVersion">アプリケーションバージョン</param>
		LobbyManager(
			const wchar_t* appID,
			const wchar_t* appVersion
		);
		/// <summary>
		/// ロビーの更新処理
		/// </summary>
		void Update();
	private:
		// receive and print out debug out here
		void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
		// implement your error-handling here
		void connectionErrorReturn(int errorCode) override;
		void clientErrorReturn(int errorCode) override;
		void warningReturn(int warningCode) override;
		void serverErrorReturn(int errorCode) override;
		// events, triggered by certain operations of all players in the same room
		void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player) override;
		void leaveRoomEventAction(int playerNr, bool isInactive) override;
		void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;
		// callbacks for operations on PhotonLoadBalancing server
		void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;
		void disconnectReturn(void) override;
		void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString) override;
		void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
		void joinLobbyReturn(void) override;
		void leaveLobbyReturn(void) override;
	private:
		ExitGames::LoadBalancing::Client m_mLoadBalancingClient;
	};
}