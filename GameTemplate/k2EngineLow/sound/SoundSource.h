/// <summary>
/// 音源クラス。
/// </summary>
#pragma once
#include "WaveFile.h"
#include "SoundEngine.h"

namespace nsK2EngineLow {
	/// <summary>
	/// 音源クラス。
	/// サウンドを再生するにはまず、g_soundEngine->ResistWaveFileBank()で波形データを読み込んでください。
	/// SoundSource::Initで登録した番号を指定して、SoundSource::Playでループフラグを指定してください。
	/// このクラスのインスタンスをゲームオブジェクトマネージャーに登録した場合、ループ再生ではない場合は再生が完了すると。
	/// 自動的に登録解除されます。ループ再生の場合はユーザーが明示的に登録を解除する必要があります。
	/// </summary>
	class SoundSource : public IGameObject {
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		SoundSource();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~SoundSource();
		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="number">g_soundEngine->ResistWaveFileBankで登録した波形データの番号。</param>
		/// <param name="is3DSound">3Dサウンドならtrue。</param>
		void Init(const int number, bool is3DSound = false);
		/// <summary>
		/// 開放。デストラクタから自動的に呼ばれます。明示的に開放を行いたい場合は使用して下さい。
		/// </summary>
		void Release();
		/// <summary>
		/// 再生。
		/// ループフラグをfalseにした場合はワンショット再生になります。
		/// ワンショット再生のインスタンスを保持していると、破棄されたインスタンスに。
		/// アクセスしてしまう可能性があるので、保持しないことをお勧めします。
		/// 逆にループフラグがtrueの場合は明示的にインスタンスを破棄する必要があるので。
		/// インスタンスを保持する必要があります。
		/// </summary>
		/// <param name="isLoop">ループフラグ。</param>
		void Play(bool isLoop);
		/// <summary>
		/// 停止。
		/// </summary>
		void Stop()
		{
			m_sourceVoice->Stop();
			m_isPlaying = false;
		}
		/// <summary>
		/// 一時停止。SoundSource::Playで提示した箇所から再生されます。
		/// </summary>
		void Pause()
		{
			m_sourceVoice->Stop(/*XAUDIO2_PLAY_TAILS*/);
		}
		/// <summary>
		/// 再生中？。
		/// </summary>
		/// <returns>trueなら再生中。</returns>
		bool IsPlaying() const
		{
			return m_isPlaying;
		}
		/// <summary>
		/// ボリュームを設定。
		/// </summary>
		/// <param name="vol">ボリューム。</param>
		void SetVolume(float vol)
		{
			m_sourceVoice->SetVolume(vol);
		}
		/// <summary>
		/// ボリュームを取得。
		/// </summary>
		/// <returns>ボリューム。</returns>
		float GetVolume() const
		{
			float vol;
			m_sourceVoice->GetVolume(&vol);
			return vol;
		}
		/// <summary>
		/// 音源の座標を設定。
		/// 3Dサウンドの時に必要になります。
		/// 2Dサウンドでは無視されます。
		/// </summary>
		/// <param name="pos">音源の座標。</param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
			if (m_isSetPositionFirst) {
				m_lastFramePosition = m_position;
				m_isSetPositionFirst = false;
			}
		}
		/// <summary>
		/// 音源の座標を取得。
		/// </summary>
		/// <returns>音源の座標。</returns>
		Vector3 GetPosition() const
		{
			return m_position;
		}
		/// <summary>
		/// 音源の移動速度を取得。
		/// </summary>
		/// <returns>音源の移動速度。</returns>
		Vector3 GetVelocity() const
		{
			return m_velocity;
		}
		/// <summary>
		/// ループフラグを取得。
		/// </summary>
		/// <returns>trueならループする。</returns>
		bool GetLoopFlag() const
		{
			return m_isLoop;
		}
		/// <summary>
		/// ボイスの周波数調整比。
		/// 詳細はIXAudio2SourceVoiceのSetFrequencyRatioを参照してください。
		/// </summary>
		/// <param name="ratio">周波数比。</param>
		void SetFrequencyRatio(float ratio)
		{
			if (m_sourceVoice != nullptr) {
				m_sourceVoice->SetFrequencyRatio(ratio);
			}
		}
		IXAudio2SourceVoice* GetXAudio2SourceVoice()
		{
			return m_sourceVoice;
		}
		/// <summary>
		/// 入力チャンネル数を取得。
		/// </summary>
		/// <returns>入力チャンネル数。</returns>
		int GetNumInputChannel()const
		{
			return m_waveFile->GetFormat()->nChannels;
		}
		FLOAT32* GetEmitterAzimuths()
		{
			return m_emitterAzimuths;
		}
		FLOAT32* GetMatrixCoefficients()
		{
			return m_matrixCoefficients;
		}
		X3DAUDIO_DSP_SETTINGS* GetDspSettings()
		{
			return &m_dspSettings;
		}
	private:
		/// <summary>
		/// 初期化。
		/// </summary>
		void InitCommon();
		/// <summary>
		/// オンメモリ再生中の更新処理。
		/// </summary>
		void UpdateOnMemory();
		void Play(char* buff, unsigned int bufferSize);
		void Remove3DSound();
		/// <summary>
		/// 更新。
		/// </summary>
		void Update() override;
	private:
		std::shared_ptr<WaveFile>		m_waveFile;						//波形データ。
		IXAudio2SourceVoice* m_sourceVoice = nullptr;		//ソースボイス。
		bool							m_isLoop = false;				//ループフラグ。
		bool							m_isPlaying = false;			//再生中フラグ。
		unsigned int					m_currentBufferingSize = 0;		//現在のバッファリングのサイズ。
		unsigned int					m_readStartPos = 0;				//読み込み開始位置。
		unsigned int					m_ringBufferSize = 0;			//リングバッファのサイズ。
		bool							m_is3DSound = false;			//3Dサウンド？
		Vector3							m_position = Vector3::Zero;		//音源の座標。3Dサウンドの時に必要。
		Vector3							m_lastFramePosition = Vector3::Zero;//音源の1フレーム前の座標。3Dサウンドの時に必要。
		Vector3							m_velocity = Vector3::Zero;		//速度。3Dサウンドの時に必要・
		FLOAT32							m_emitterAzimuths[INPUTCHANNELS];
		FLOAT32							m_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
		X3DAUDIO_DSP_SETTINGS			m_dspSettings;
		bool							m_isSetPositionFirst = true;	//一番最初のsetPosition?
		bool							m_isAvailable = false;			//インスタンスが利用可能？
	};
}