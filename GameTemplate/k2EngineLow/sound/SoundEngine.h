/// <summary>
/// �T�E���h�G���W���B
/// </summary>
#pragma once
#include "WaveFileBank.h"

namespace nsK2EngineLow {
#define INPUTCHANNELS 2  // number of source channels
#define OUTPUTCHANNELS 8	//�ő�o�̓`�����l�����B

	class WaveFile;
	class SoundSource;
	/// <summary>
	/// �T�E���h�G���W���B
	/// </summary>
	class SoundEngine : public Noncopyable {
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		SoundEngine();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~SoundEngine();
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
		/// <summary>
		/// �J���B
		/// </summary>
		void Release();
		/// <summary>
		/// �X�V�B
		/// </summary>
		void Update();
		/// <summary>
		/// 3D�T�E���h�\�[�X��ǉ��B
		/// </summary>
		/// <param name="s">3D�T�E���h�\�[�X�B</param>
		void Add3DSoundSource(SoundSource* s)
		{
			m_3dSoundSource.push_back(s);
		}
		/// <summary>
		/// 3D�T�E���h�\�[�X���폜�B
		/// </summary>
		/// <param name="s">3D�T�E���h�\�[�X�B</param>
		void Remove3DSoundSource(SoundSource* s)
		{
			auto it = std::find(m_3dSoundSource.begin(), m_3dSoundSource.end(), s);
			if (it != m_3dSoundSource.end()) {
				m_3dSoundSource.erase(it);
			}
		}
		/// <summary>
		/// �T�E���h���X�i�[�̍��W��ݒ�B
		/// 3D�������g���Ă��鎞�̌v�Z�Ɏg�p����܂��B
		/// </summary>
		/// <param name="pos">�T�E���h���X�i�[�̍��W�B</param>
		void SetListenerPosition(const Vector3& pos)
		{
			m_listenerPosition = pos;
		}
		/// <summary>
		/// �T�E���h���X�i�[�̑O������ݒ�B
		/// 3D�������g���Ă��鎞�̌v�Z�Ɏg�p����܂��B
		/// </summary>
		/// <param name="front">�T�E���h���X�i�[�̑O�����B</param>
		void SetListenerFront(Vector3 front)
		{
			m_listener.OrientFront.x = front.x;
			m_listener.OrientFront.y = front.y;
			m_listener.OrientFront.z = front.z;
		}
		/// <summary>
		/// �T�E���h���X�i�[�̏������ݒ�B
		/// 3D�������g���Ă��鎞�̌v�Z�Ɏg�p����܂��B
		/// </summary>
		/// <param name="up">�T�E���h���X�i�[�̏�����B</param>
		void SetListenerUp(Vector3 up)
		{
			m_listener.OrientTop.x = up.x;
			m_listener.OrientTop.y = up.y;
			m_listener.OrientTop.z = up.z;
		}
		/// <summary>
		/// XAudio2�̃\�[�X�{�C�X��ǉ��B
		/// </summary>
		/// <param name="waveFile">�g�`�f�[�^�B</param>
		/// <param name="is3DSound">true�Ȃ�3D�T�E���h�B</param>
		/// <returns>XAudio2�̃\�[�X�{�C�X�B</returns>
		IXAudio2SourceVoice* CreateXAudio2SourceVoice(WaveFile* waveFile, bool is3DSound);
		IXAudio2MasteringVoice* GetMasteringVoice()
		{
			return m_masteringVoice;
		}
		IXAudio2SubmixVoice* GetSubmixVoice()
		{
			return m_submixVoice;
		}
		/// <summary>
		/// �o�̓`�����l���̐����擾�B
		/// </summary>
		/// <returns></returns>
		int GetNumChannel() const
		{
			return m_nChannels;
		}
		/// <summary>
		/// �g�`�f�[�^�o���N���擾�B
		/// </summary>
		/// <returns></returns>
		WaveFileBank& GetWaveFileBank()
		{
			return m_waveFileBank;
		}
		/// <summary>
		/// �g�`�f�[�^��o�^����B
		/// </summary>
		void ResistWaveFileBank(int number, const char* filePath)
		{
			m_waveFileBank.Resist(number, filePath);
		}
	private:
		IXAudio2* m_xAudio2 = nullptr;
		IXAudio2MasteringVoice* m_masteringVoice = nullptr;
		X3DAUDIO_HANDLE				m_hx3DAudio;						//3D�I�[�f�B�I�̃n���h���B
		IUnknown* m_reverbEffect = nullptr;			//���o�[�v�G�t�F�N�g�B
		IXAudio2SubmixVoice* m_submixVoice = nullptr;			//�T�u�~�b�N�X�{�C�X�B
		DWORD						m_channelMask = 0;					//�I�[�f�B�I�`�����l���}�X�N�B
		DWORD						m_nChannels = 0;					//�`�����l�����B
		Vector3						m_listenerPosition = Vector3::Zero;	//���X�i�[�̍��W�B
		float						m_fListenerAngle = 0.0f;
		bool						m_UseListenerCone = false;
		bool						m_fUseInnerRadius = false;
		bool						m_useRedirectToLFE = false;
		bool						m_isInited = false;
		X3DAUDIO_LISTENER			m_listener;							//�T�E���h���X�i�[�B
		std::list<SoundSource*>		m_3dSoundSource;					//3D�T�E���h�̃T�E���h�\�[�X�B
		X3DAUDIO_CONE				m_emitterCone;
		WaveFileBank				m_waveFileBank;						//�g�`�f�[�^�̃o���N�B
	};

	extern SoundEngine* g_soundEngine;	//TK�G���W���B
}