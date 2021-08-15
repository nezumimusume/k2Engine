/// <summary>
/// �����N���X�B
/// </summary>
#pragma once
#include "WaveFile.h"
#include "SoundEngine.h"

namespace nsK2EngineLow {
	/// <summary>
	/// �����N���X�B
	/// �T�E���h���Đ�����ɂ͂܂��Ag_soundEngine->ResistWaveFileBank()�Ŕg�`�f�[�^��ǂݍ���ł��������B
	/// SoundSource::Init�œo�^�����ԍ����w�肵�āASoundSource::Play�Ń��[�v�t���O���w�肵�Ă��������B
	/// ���̃N���X�̃C���X�^���X���Q�[���I�u�W�F�N�g�}�l�[�W���[�ɓo�^�����ꍇ�A���[�v�Đ��ł͂Ȃ��ꍇ�͍Đ�����������ƁB
	/// �����I�ɓo�^��������܂��B���[�v�Đ��̏ꍇ�̓��[�U�[�������I�ɓo�^����������K�v������܂��B
	/// </summary>
	class SoundSource : public IGameObject {
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		SoundSource();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~SoundSource();
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="number">g_soundEngine->ResistWaveFileBank�œo�^�����g�`�f�[�^�̔ԍ��B</param>
		/// <param name="is3DSound">3D�T�E���h�Ȃ�true�B</param>
		void Init(const int number, bool is3DSound = false);
		/// <summary>
		/// �J���B�f�X�g���N�^���玩���I�ɌĂ΂�܂��B�����I�ɊJ�����s�������ꍇ�͎g�p���ĉ������B
		/// </summary>
		void Release();
		/// <summary>
		/// �Đ��B
		/// ���[�v�t���O��false�ɂ����ꍇ�̓����V���b�g�Đ��ɂȂ�܂��B
		/// �����V���b�g�Đ��̃C���X�^���X��ێ����Ă���ƁA�j�����ꂽ�C���X�^���X�ɁB
		/// �A�N�Z�X���Ă��܂��\��������̂ŁA�ێ����Ȃ����Ƃ������߂��܂��B
		/// �t�Ƀ��[�v�t���O��true�̏ꍇ�͖����I�ɃC���X�^���X��j������K�v������̂ŁB
		/// �C���X�^���X��ێ�����K�v������܂��B
		/// </summary>
		/// <param name="isLoop">���[�v�t���O�B</param>
		void Play(bool isLoop);
		/// <summary>
		/// ��~�B
		/// </summary>
		void Stop()
		{
			m_sourceVoice->Stop();
			m_isPlaying = false;
		}
		/// <summary>
		/// �ꎞ��~�BSoundSource::Play�Œ񎦂����ӏ�����Đ�����܂��B
		/// </summary>
		void Pause()
		{
			m_sourceVoice->Stop(/*XAUDIO2_PLAY_TAILS*/);
		}
		/// <summary>
		/// �Đ����H�B
		/// </summary>
		/// <returns>true�Ȃ�Đ����B</returns>
		bool IsPlaying() const
		{
			return m_isPlaying;
		}
		/// <summary>
		/// �{�����[����ݒ�B
		/// </summary>
		/// <param name="vol">�{�����[���B</param>
		void SetVolume(float vol)
		{
			m_sourceVoice->SetVolume(vol);
		}
		/// <summary>
		/// �{�����[�����擾�B
		/// </summary>
		/// <returns>�{�����[���B</returns>
		float GetVolume() const
		{
			float vol;
			m_sourceVoice->GetVolume(&vol);
			return vol;
		}
		/// <summary>
		/// �����̍��W��ݒ�B
		/// 3D�T�E���h�̎��ɕK�v�ɂȂ�܂��B
		/// 2D�T�E���h�ł͖�������܂��B
		/// </summary>
		/// <param name="pos">�����̍��W�B</param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
			if (m_isSetPositionFirst) {
				m_lastFramePosition = m_position;
				m_isSetPositionFirst = false;
			}
		}
		/// <summary>
		/// �����̍��W���擾�B
		/// </summary>
		/// <returns>�����̍��W�B</returns>
		Vector3 GetPosition() const
		{
			return m_position;
		}
		/// <summary>
		/// �����̈ړ����x���擾�B
		/// </summary>
		/// <returns>�����̈ړ����x�B</returns>
		Vector3 GetVelocity() const
		{
			return m_velocity;
		}
		/// <summary>
		/// ���[�v�t���O���擾�B
		/// </summary>
		/// <returns>true�Ȃ烋�[�v����B</returns>
		bool GetLoopFlag() const
		{
			return m_isLoop;
		}
		/// <summary>
		/// �{�C�X�̎��g��������B
		/// �ڍׂ�IXAudio2SourceVoice��SetFrequencyRatio���Q�Ƃ��Ă��������B
		/// </summary>
		/// <param name="ratio">���g����B</param>
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
		/// ���̓`�����l�������擾�B
		/// </summary>
		/// <returns>���̓`�����l�����B</returns>
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
		/// �������B
		/// </summary>
		void InitCommon();
		/// <summary>
		/// �I���������Đ����̍X�V�����B
		/// </summary>
		void UpdateOnMemory();
		void Play(char* buff, unsigned int bufferSize);
		void Remove3DSound();
		/// <summary>
		/// �X�V�B
		/// </summary>
		void Update() override;
	private:
		std::shared_ptr<WaveFile>		m_waveFile;						//�g�`�f�[�^�B
		IXAudio2SourceVoice* m_sourceVoice = nullptr;		//�\�[�X�{�C�X�B
		bool							m_isLoop = false;				//���[�v�t���O�B
		bool							m_isPlaying = false;			//�Đ����t���O�B
		unsigned int					m_currentBufferingSize = 0;		//���݂̃o�b�t�@�����O�̃T�C�Y�B
		unsigned int					m_readStartPos = 0;				//�ǂݍ��݊J�n�ʒu�B
		unsigned int					m_ringBufferSize = 0;			//�����O�o�b�t�@�̃T�C�Y�B
		bool							m_is3DSound = false;			//3D�T�E���h�H
		Vector3							m_position = Vector3::Zero;		//�����̍��W�B3D�T�E���h�̎��ɕK�v�B
		Vector3							m_lastFramePosition = Vector3::Zero;//������1�t���[���O�̍��W�B3D�T�E���h�̎��ɕK�v�B
		Vector3							m_velocity = Vector3::Zero;		//���x�B3D�T�E���h�̎��ɕK�v�E
		FLOAT32							m_emitterAzimuths[INPUTCHANNELS];
		FLOAT32							m_matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];
		X3DAUDIO_DSP_SETTINGS			m_dspSettings;
		bool							m_isSetPositionFirst = true;	//��ԍŏ���setPosition?
		bool							m_isAvailable = false;			//�C���X�^���X�����p�\�H
	};
}