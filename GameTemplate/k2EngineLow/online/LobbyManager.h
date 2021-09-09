#pragma once

#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Client.h"

namespace nsK2EngineLow {
	/// <summary>
	/// ���r�[�}�l�[�W���B
	/// </summary>
	/// <remark>
	/// PhotonCloudSDK�𗘗p�������r�[�}�l�[�W���[�N���X�B
	/// �{�N���X�𗘗p���邽�߂ɂ́APhotonCloud�ɂă��[�U�[�A�J�E���g���쐬���āA
	/// �쐬����A�v����o�^����K�v������܂��B
	/// ���[�U�[�A�J�E���g�̍쐬�A�A�v���̓o�^���܂��̏ꍇ�́A���LURL�ɃW�����v����
	/// �o�^���s���Ă��������B
	/// https://www.photonengine.com/ja/
	/// </remark>
	class LobbyManager : ExitGames::LoadBalancing::Listener, Noncopyable{
	public:
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <remark>
		/// �A�v���P�[�V����ID�A�A�v���P�[�V�����o�[�W������Photo
		/// </remark>
		/// <param name="appID">�A�v���P�[�V����ID</param>
		/// <param name="appVersion">�A�v���P�[�V�����o�[�W����</param>
		LobbyManager(
			const wchar_t* appID,
			const wchar_t* appVersion
		);
		/// <summary>
		/// ���r�[�̍X�V����
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