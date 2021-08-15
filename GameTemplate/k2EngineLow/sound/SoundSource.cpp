/*!
 * @brief	�����N���X�B
 */
#include "k2EngineLowPreCompile.h"
#include "SoundEngine.h"
#include "SoundSource.h"

namespace nsK2EngineLow {

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
		m_dspSettings.DstChannelCount = g_soundEngine->GetNumChannel();
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
		m_waveFile = g_soundEngine->GetWaveFileBank().FindWaveFile(number);
		if (!m_waveFile) {
			//TODO �����ɃG���[���b�Z�[�W�B

			return;
		}
		//�T�E���h�{�C�X�\�[�X���쐬�B
		m_sourceVoice = g_soundEngine->CreateXAudio2SourceVoice(m_waveFile.get(), is3DSound);
		if (is3DSound) {
			g_soundEngine->Add3DSoundSource(this);
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
			//�Đ����̂��̂��ĊJ����B
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

	void SoundSource::Release()
	{
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
			//�Đ��J�n�B
			if (FAILED(m_sourceVoice->SubmitSourceBuffer(&buffer))) {
				Release();
				//TK_LOG("Failed m_sourceVoice->SubmitSourceBuffer");
				return;
			}

		}
	}

	void SoundSource::Remove3DSound()
	{
		if (m_is3DSound) {
			g_soundEngine->Remove3DSoundSource(this);
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
				//���[�v�B
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
		}
		//�I���������Đ����̍X�V�����B
		UpdateOnMemory();
		if (m_is3DSound == true) {
			//�����̈ړ����x���X�V�B
			m_velocity.Subtract(m_position, m_lastFramePosition);
			//TODO 1.0f/60.0f�̂Ƃ�����t���[�����ԂɕύX����B
			m_velocity.Div(g_gameTime->GetFrameDeltaTime());
			m_lastFramePosition = m_position;
		}

	}
}