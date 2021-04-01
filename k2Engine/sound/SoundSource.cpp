/*!
 * @brief	音源クラス。
 */
#include "k2EnginePreCompile.h"
#include "SoundEngine.h"
#include "SoundSource.h"



		SoundSource::SoundSource()
		{
			memset(m_emitterAzimuths, 0, sizeof(m_emitterAzimuths));
			memset(m_matrixCoefficients, 0, sizeof(m_matrixCoefficients));
		}
		SoundSource::~SoundSource()
		{
			Release();
		}
		void SoundSource::InitCommon()
		{
			m_dspSettings.SrcChannelCount = INPUTCHANNELS;
			m_dspSettings.DstChannelCount = SoundEngine().GetNumChannel();
			m_dspSettings.pMatrixCoefficients = m_matrixCoefficients;
			m_dspSettings.pDelayTimes = nullptr;
			m_dspSettings.DopplerFactor = 1.0f;
			m_dspSettings.LPFDirectCoefficient = 0.82142854f;
			m_dspSettings.LPFReverbCoefficient = 0.75f;
			m_dspSettings.ReverbLevel = 0.69114286f;
			m_dspSettings.EmitterToListenerAngle = 0.0f;
			m_dspSettings.EmitterToListenerDistance = 10.0f;
			m_dspSettings.EmitterVelocityComponent = 0.0f;
			m_dspSettings.ListenerVelocityComponent = 0.0f;
		}

		void SoundSource::Init(const int number, bool is3DSound)
		{
			m_isAvailable = false;
			m_waveFile = SoundEngine().GetWaveFileBank().FindWaveFile(number);
			if (!m_waveFile) {
				//TODO ここにエラーメッセージ。

				return;
			}
			//サウンドボイスソースを作成。
			m_sourceVoice = SoundEngine().CreateXAudio2SourceVoice(m_waveFile.get(), is3DSound);
			if (is3DSound) {
				SoundEngine().Add3DSoundSource(this);
			}
			InitCommon();

			m_is3DSound = is3DSound;
			m_isAvailable = true;
		}

		void SoundSource::Play(bool isLoop)
		{
			if (m_isAvailable == false) {

				return;
			}
			if (m_isPlaying) {
				//再生中のものを再開する。
				m_sourceVoice->Start(0);
			}
			else {
				m_sourceVoice->FlushSourceBuffers();
				m_sourceVoice->Start(0);
				Play(m_waveFile->GetReadBuffer(), m_waveFile->GetSize());
				m_isPlaying = true;
			}
			m_isLoop = isLoop;
		}

		/*void SoundSource::Init(char* filePath, bool is3DSound)
		{
			m_isAvailable = false;
			m_waveFile = SoundEngine().GetWaveFileBank().FindWaveFile(0, filePath);
			if (!m_waveFile) {
				m_waveFile.reset(new WaveFile);
				bool result = m_waveFile->Open(filePath);
				if (result == false) {
					//waveファイルの読み込みに失敗。
					SoundEngine().GetWaveFileBank().UnregistWaveFile(0, m_waveFile);
					m_waveFile.reset();
					return;
				}
				m_waveFile->AllocReadBuffer(m_waveFile->GetSize());	//waveファイルのサイズ分の読み込みバッファを確保する。
				SoundEngine().GetWaveFileBank().RegistWaveFile(0, m_waveFile);
				unsigned int dummy;
				m_waveFile->Read(m_waveFile->GetReadBuffer(), m_waveFile->GetSize(), &dummy);
				m_waveFile->ResetFile();

			}

			//サウンドボイスソースを作成。
			m_sourceVoice = SoundEngine().CreateXAudio2SourceVoice(m_waveFile.get(), is3DSound);
			if (is3DSound) {
				SoundEngine().Add3DSoundSource(this);
			}
			InitCommon();

			m_is3DSound = is3DSound;
			m_isAvailable = true;
		}*/
		/*void SoundSource::Init(const int nameKey, bool is3DSound)
		{
			m_isAvailable = false;
			m_waveFile = SoundEngine().GetWaveFileBank().FindWaveFile(0, nameKey);
			if (!m_waveFile) {
				m_waveFile.reset(new WaveFile);
				m_waveFile->Open(nameKey.GetName());
				SoundEngine().GetWaveFileBank().RegistWaveFile(0, m_waveFile);
				m_waveFile->AllocReadBuffer(m_waveFile->GetSize());	//waveファイルのサイズ分の読み込みバッファを確保する。
				SoundEngine().GetWaveFileBank().RegistWaveFile(0, m_waveFile);
				unsigned int dummy;
				m_waveFile->Read(m_waveFile->GetReadBuffer(), m_waveFile->GetSize(), &dummy);
				m_waveFile->ResetFile();
			}
			//サウンドボイスソースを作成。
			m_sourceVoice = SoundEngine().CreateXAudio2SourceVoice(m_waveFile.get(), is3DSound);
			if (is3DSound) {
				SoundEngine().Add3DSoundSource(this);
			}
			InitCommon();

			m_is3DSound = is3DSound;
			m_isAvailable = true;
		}
		*/
		/*void SoundSource::InitStreaming(char* filePath, bool is3DSound, unsigned int ringBufferSize, unsigned int bufferSize)
		{
			m_isAvailable = false;
			//ストリーミングはWaveFileの使いまわしはできない。
			m_waveFile.reset(new WaveFile);
			m_waveFile->Open(filePath);

			m_isStreaming = true;
			m_streamingBufferSize = bufferSize;
			m_waveFile->AllocReadBuffer(ringBufferSize);
			m_ringBufferSize = ringBufferSize;
			m_readStartPos = 0;
			m_currentBufferingSize = 0;
			//サウンドボイスソースを作成。
			m_sourceVoice = SoundEngine().CreateXAudio2SourceVoice(m_waveFile.get(), is3DSound);
			if (is3DSound) {
				SoundEngine().Add3DSoundSource(this);
			}
			InitCommon();

			m_is3DSound = is3DSound;
			m_isAvailable = true;
		}*/
		void SoundSource::Release()
		{
			if (m_isStreaming) {
				m_waveFile->Release();
			}
			if (m_sourceVoice != nullptr) {
				m_sourceVoice->DestroyVoice();
				m_sourceVoice = nullptr;
			}
			Remove3DSound();
			DeleteGO(this);
		}
		void SoundSource::Play(char* buff, unsigned int bufferSize)
		{
			XAUDIO2_BUFFER buffer = { 0 };
			buffer.pAudioData = (BYTE*)buff;
			buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
			buffer.AudioBytes = bufferSize;
			if (m_sourceVoice != nullptr && bufferSize > 0) {
				//再生開始。
				if (FAILED(m_sourceVoice->SubmitSourceBuffer(&buffer))) {
					Release();
					//TK_LOG("Failed m_sourceVoice->SubmitSourceBuffer");
					return;
				}

			}
		}

		void SoundSource::StartStreamingBuffring()
		{
			char* readStartBuff = m_waveFile->GetReadBuffer();
			m_readStartPos += m_currentBufferingSize;
			if (m_readStartPos + m_streamingBufferSize >= m_ringBufferSize) {
				//リングバッファのサイズを超える。
				//循環する。
				m_readStartPos = 0;
			}
			m_waveFile->ReadAsync(&readStartBuff[m_readStartPos], m_streamingBufferSize, &m_currentBufferingSize);
			m_streamingState = enStreamingBuffering;
		}
		
		void SoundSource::UpdateStreaming()
		{
			if (!m_isPlaying) {
				return;
			}
			if (m_streamingState == enStreamingBuffering) {
				//バッファリング中。
				if (m_waveFile->IsReadEnd()) {
					//バッファリングが終わった。
					m_streamingState = enStreamingQueueing;
				}
			}
			if (m_streamingState == enStreamingQueueing) {
				//キューイング中。
				XAUDIO2_VOICE_STATE state;
				m_sourceVoice->GetState(&state);
				if (state.BuffersQueued <= 2) {	//キューイングされているバッファが２以下になったらキューイングできる。
					char* buff = m_waveFile->GetReadBuffer();
					Play(&buff[m_readStartPos], m_currentBufferingSize);
					if (m_currentBufferingSize == 0) {
						//読み込んだサイズが０ということは末端まで読み込みが終わったということ。
						if (m_isLoop) {
							//ループする？
							//waveファイルの読み込み位置をリセットしてバッファリング再開。
							m_waveFile->ResetFile();
							StartStreamingBuffring();
						}
						else {
							//ループしない場合はキューが空になったら再生終了。
							if (state.BuffersQueued == 0) {
								//再生終了。
								m_isPlaying = false;
								DeleteGO(this);
								Remove3DSound();
							}
						}
					}
					else {
						//次をバッファリングする。
						StartStreamingBuffring();
					}
				}
			}
		}
		void SoundSource::Remove3DSound()
		{
			if (m_is3DSound) {
				SoundEngine().Remove3DSoundSource(this);
				m_is3DSound = false;
			}
		}
		void SoundSource::UpdateOnMemory()
		{
			if (!m_isPlaying) {
				return;
			}
			XAUDIO2_VOICE_STATE state;
			m_sourceVoice->GetState(&state);
			if (state.BuffersQueued <= 0) {
				m_isPlaying = false;
				if (m_isLoop) {
					//ループ。
					Play(m_isLoop);
				}
				else {
					DeleteGO(this);
					Remove3DSound();
				}
			}
		}
		void SoundSource::Update()
		{
			if (m_isAvailable == false) {
				return;
				{
					//オンメモリ再生中の更新処理。
					UpdateOnMemory();
				}
				if (m_is3DSound == true) {
					//音源の移動速度を更新。
					m_velocity.Subtract(m_position, m_lastFramePosition);
					//TODO 1.0f/60.0fのところをフレーム時間に変更する。
					m_velocity.Div(1.0f / 60.0f);
					m_lastFramePosition = m_position;
				}
			}
		}