/*!
 * @brief	サウンドエンジン
 */

#pragma once
#include "WaveFileBank.h"

#define INPUTCHANNELS 2  // number of source channels
#define OUTPUTCHANNELS 8	//最大出力チャンネル数。

	class WaveFile;

		class SoundSource;
	
	/*!
	 * @brief	サウンドエンジン。
	 */
	class SoundEngine{
	public:
		/*!
		 * @brief	コンストラクタ。
		 */
		SoundEngine();
		/*!
		 * @brief	デストラクタ。
		 */
		~SoundEngine();
		/*!
		 * @brief	初期化。
		 */
		void Init();
		/*!
		 * @brief	開放。
		 */
		void Release();
		/*!
		* @brief	更新。
		*/
		void Update();
		/*!
		* @brief	3Dサウンドソースを追加。
		*/
		void Add3DSoundSource(SoundSource* s)
		{
			m_3dSoundSource.push_back(s);
		}
		/*!
		* @brief	3Dサウンドソースを削除。
		*/
		void Remove3DSoundSource(SoundSource* s)
		{
			auto it = std::find(m_3dSoundSource.begin(), m_3dSoundSource.end(), s);
			if (it != m_3dSoundSource.end()) {
				m_3dSoundSource.erase(it);
			}
		}
		/*!
		* @brief	サウンドリスナーの座標を設定。
		* @details
		*  3D音源を使っている時の計算に使用されます。
		*/
		void SetListenerPosition(const Vector3& pos)
		{
			m_listenerPosition = pos;
		}
		/*!
		* @brief	サウンドリスナーの前方向を設定。
		* @details
		*  3D音源を使っている時の計算に使用されます。
		*/
		void SetListenerFront(Vector3 front)
		{
			m_listener.OrientFront.x = front.x;
			m_listener.OrientFront.y = front.y;
			m_listener.OrientFront.z = front.z;
		}
		/*!
		* @brief	サウンドリスナーの上方向を設定。
		* @details
		*  3D音源を使っている時の計算に使用されます。
		*/
		void SetListenerUp(Vector3 up)
		{
			m_listener.OrientTop.x = up.x;
			m_listener.OrientTop.y = up.y;
			m_listener.OrientTop.z = up.z;
		}
		/*!
		* @brief	XAudio2のソースボイスを作成。
		*/
		IXAudio2SourceVoice* CreateXAudio2SourceVoice(WaveFile* waveFile, bool is3DSound);
		IXAudio2MasteringVoice* GetMasteringVoice()
		{
			return m_masteringVoice;
		}
		IXAudio2SubmixVoice* GetSubmixVoice()
		{
			return m_submixVoice;
		}
		/*!
		* @brief	出力チャンネルの数を取得。
		*/
		int GetNumChannel() const
		{
			return m_nChannels;
		}
		/*!
		* @brief	波形データバンクを取得。
		*/
		WaveFileBank& GetWaveFileBank()
		{
			return m_waveFileBank;
		}
	private:
		IXAudio2* m_xAudio2 = nullptr;
		IXAudio2MasteringVoice* m_masteringVoice = nullptr;
		X3DAUDIO_HANDLE m_hx3DAudio;					//!<3Dオーディオのハンドル。
		IUnknown* m_reverbEffect = nullptr;				//!<リバープエフェクト。
		IXAudio2SubmixVoice* m_submixVoice = nullptr;	//!<サブミックスボイス。
		DWORD m_channelMask = 0;						//!<オーディオチャンネルマスク。
		DWORD m_nChannels = 0;							//!<チャンネル数。
		Vector3 m_listenerPosition = Vector3::Zero;	//!<リスナーの座標。
		float m_fListenerAngle = 0.0f;
		bool m_UseListenerCone = false;
		bool m_fUseInnerRadius = false;
		bool m_useRedirectToLFE = false;
		bool m_isInited = false;
		X3DAUDIO_LISTENER m_listener;					//!<サウンドリスナー。
		std::list<SoundSource*>	m_3dSoundSource;	//!<3Dサウンドのサウンドソース。
		X3DAUDIO_CONE	m_emitterCone;
		WaveFileBank m_waveFileBank;					//!<波形データのバンク。
	};
