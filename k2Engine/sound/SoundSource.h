/*!
 * @brief	音源クラス。
 */

#pragma once

#include "WaveFile.h"


		/*!
		 * @brief	音源クラス。
		 *@details
		 * このクラスには波形データをオンメモリに乗せて再生する方法と、ストリーミング再生する方法があります。</br>
		 * オンメモリに乗せる場合はSoundSource::Initを使って初期化を行ってください。</br>
		 * ストリーミング再生を行う場合はSoundSource::InitStreamingを使用してください。</br>
		 * オンメモリでの再生は波形データを全てメインメモリ上に乗せて再生を行う方法です。</br>
		 * ストリーミング再生はメディア(DVD、HDDなどの外部補助記憶装置)から少しづつ読み込みを行って、</br>
		 * 逐次実行していく方法です。ストリーミング再生は必要な分だけ読み込みを行い(バッファリング)を行い、</br>
		 * 再生が完了するとそのバッファを破棄します。このため、少ないメモリで大きな波形データを再生することが可能です。</br>
		 * オンメモリ再生は読み込みのオーバーヘッドが最初だけになるが、ストリーミング再生は常に読み込みのオーバーヘッドが</br>
		 * 発生するためパフォマーンスが低下します。</br>
		 * 一般的にオンメモリ再生は効果音などのようなサイズの小さなSEの再生に適しています。</br>
		 * ストリーミング再生は大きなサイズのBGMの再生に適しています。</br>
		 *
		 * このクラスのインスタンスをゲームオブジェクトマネージャーに登録した場合、ループ再生ではない場合は再生が完了すると</br>
		 * 自動的に登録解除されます。ループ再生の場合はユーザーが明示的に登録を解除する必要があります。
		 *@code
			//初期化
			void Init()
			{
				SoundSource* ss = NewGO<SoundSource>(0);
				ss->Init("test.wave");				//こちらで初期化するとオンメモリ再生。
				//soundSource.InitStreaming("test.wave");	//こちらで初期化するとストリーミング再生。
				ss->Play(true);						//これはループ再生。
			}
	
		 *@endcode
		 */
		class SoundSource : public IGameObject {
		public:
			/*!
			 * @brief	コンストラクタ。
			 */
			SoundSource();
			/*!
			 * @brief	デストラクタ。
			 */
			~SoundSource();
			/*!
			 * @brief	初期化。
			 *@details
			 * オンメモリ再生向けの初期化。
			 *@param[in]	filePath	ファイルパス。対応しているファイルフォーマット(*.wave)
			 *@param[in]	is3DSound	3Dサウンド？
			 */
			void Init(wchar_t* filePath, bool is3DSound = false);
			/*!
			 * @brief	初期化。
			 *@details
			 * オンメモリ再生向けの初期化。こちらを使う方がちょっとだけ速い。
			 *@param[in]	nameKey		名前キー。
			 *@param[in]	is3DSound	3Dサウンド？
			 */
			void Init(const WNameKey& nameKey, bool is3DSound = false);
			/*!
			* @brief	初期化。
			*@details
			* ストリーミング再生向けの初期化。
			*@details
			* リングバッファにストリーミング読み込みを行って、再生を行っていきます。</br>
			* 一度に読み込まれるデータの最大サイズはbufferingSizeです。</br>
			* 読み込まれたデータはリングバッファにコピーされていきます。</br>
			*@param[in]	filePath		ファイルパス。対応しているファイルフォーマット(*.wave)
			*@param[in] is3DSound		3Dサウンド？
			*@param[in] ringBufferSize	リングバッファのサイズ。(bufferSizeの倍数になっていると無駄なく活用できます。)
			*@param[in]	bufferSize		ストリーミングの最大バッファリングサイズ。
			*/
			void InitStreaming(wchar_t* filePath, bool is3DSound = false, unsigned int ringBufferSize = 3 * 1024 * 1024, unsigned int bufferingSize = 1024 * 512);
			/*!
			* @brief	開放。
			*@details
			* デストラクタから自動的に呼ばれます。明示的に開放を行いたい場合に使用してください。
			*/
			void Release();

			/*!
			* @brief	再生。
			*@param[in]	isLoop		ループ再生フラグ。
			* このフラグがfalseの場合はワンショット再生となります。
			* ワンショット再生の場合は、再生が完了すると自動的にインスタンスが破棄されます。
			* ワンショット再生のSoundSourceのインスタンスをメンバ変数などで保持していると、破棄された
			* インスタンスにアクセスしてしまう可能性があるので、保持しないことをお勧めします。
			* 再生の完了を検出したい場合などはイベントリスナーを使用してください。
			*/
			void Play(bool isLoop);
			/*!
			* @brief	停止。
			*/
			void Stop()
			{
				m_sourceVoice->Stop();
				m_isPlaying = false;
			}
			/*!
			* @brief	一時停止。
			*/
			void Pause()
			{
				m_sourceVoice->Stop(/*XAUDIO2_PLAY_TAILS*/);
			}
			/*!
			* @brief	再生中？。
			*/
			bool IsPlaying() const
			{
				return m_isPlaying;
			}
			
			/*!
			* @brief	ボリュームを設定。
			*@param[in]	vol		ボリューム。
			*/
			void SetVolume(float vol)
			{
				m_sourceVoice->SetVolume(vol);
			}
			/*!
			* @brief	ボリュームを取得。
			*/
			float GetVolume() const
			{
				float vol;
				m_sourceVoice->GetVolume(&vol);
				return vol;
			}
			/*!
			* @brief	音源の座標を設定。
			* @details
			*  3Dサウンドの時に必要になります。
			*  2Dサウンドでは無視されます。
			*@param[in] pos		音源の座標。
			*/
			void SetPosition(const Vector3& pos)
			{
				m_position = pos;
				if (m_isSetPositionFirst) {
					m_lastFramePosition = m_position;
					m_isSetPositionFirst = false;
				}
			}
			/*!
			* @brief	音源の座標を取得。
			*/
			Vector3 GetPosition() const
			{
				return m_position;
			}
			/*!
			* @brief	音源の移動速度を取得。
			*/
			Vector3 GetVelocity() const
			{
				return m_velocity;
			}
			/*!
			* @brief	ループフラグを取得。
			*/
			bool GetLoopFlag() const
			{
				return m_isLoop;
			}
			/*!
			* @brief	ボイスの周波数調整比
			*@details
			* 詳細はIXAudio2SourceVoiceのSetFrequencyRatioを参照してください。
			*@param[in]	ratio		周波数比。
			*/
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
			//入力チャンネル数を取得。
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
			void InitCommon();
			//ストリーミング再生中の更新処理。
			void UpdateStreaming();
			//オンメモリ再生中の更新処理。
			void UpdateOnMemory();
			void Play(char* buff, unsigned int bufferSize);
			/*!
			* @brief	ストリーミングバッファリングの開始。
			*/
			void StartStreamingBuffring();
			void Remove3DSound();
			/*!
			* @brief	更新。
			*/
			void Update() override;
		private:
			enum EnStreamingStatus {
				enStreamingBuffering,	//バッファリング中。
				enStreamingQueueing,	//キューイング中。
			};
			std::shared_ptr<WaveFile>	m_waveFile;					//!<波形データ。
			IXAudio2SourceVoice*		m_sourceVoice = nullptr;	//!<ソースボイス。
			bool						m_isLoop = false;			//!<ループフラグ。
			bool						m_isPlaying = false;		//!<再生中フラグ。
			bool					m_isStreaming = false;		//!<ストリーミング再生？
			unsigned int			m_streamingBufferSize = 0;	//!<ストリーミング用のバッファリングサイズ。
			unsigned int			m_currentBufferingSize = 0;	//!<現在のバッファリングのサイズ。
			unsigned int			m_readStartPos = 0;			//!<読み込み開始位置。
			unsigned int			m_ringBufferSize = 0;		//!<リングバッファのサイズ。
			EnStreamingStatus		m_streamingState = enStreamingBuffering;	//!<ストリーミングステータス。
			bool					m_is3DSound = false;		//!<3Dサウンド？
			Vector3				m_position = Vector3::Zero;	//!<音源の座標。3Dサウンドの時に必要。
			Vector3				m_lastFramePosition = Vector3::Zero;//!<音源の1フレーム前の座標。3Dサウンドの時に必要。
			Vector3				m_velocity = Vector3::Zero;	//!<速度。3Dサウンドの時に必要・
			FLOAT32 m_emitterAzimuths[INPUTCHANNELS];
			FLOAT32 m_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
			X3DAUDIO_DSP_SETTINGS m_dspSettings;
			bool m_isSetPositionFirst = true;	//!<一番最初のsetPosition?
			bool m_isAvailable = false;			//!<インスタンスが利用可能？
		};
