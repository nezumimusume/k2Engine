/// <summary>
/// wave�t�@�C���B
/// </summary>
#pragma once

#include <thread>

namespace nsK2EngineLow {
	/// <summary>
	/// Wave�t�@�C�����������߂̃N���X�B
	/// Read()�œ����ǂݍ��݁AReadAsync()�͔񓯊��ǂݍ��݂��s���܂��B
	/// ReadAsync�𗘗p�����ꍇ��WaveFile::IsReadEnd�𗘗p���ē������Ƃ��Ă��������B
	/// </summary>
	class WaveFile : public Noncopyable {
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		WaveFile();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~WaveFile();
		/// <summary>
		/// wave�t�@�C�����I�[�v���B
		/// </summary>
		/// <param name="fileName">�t�@�C�����B</param>
		/// <returns>false��Ԃ��ė�����I�[�v���Ɏ��s�B</returns>
		bool Open(const char* fileName);
		/// <summary>
		/// �g�`�f�[�^��ǂݍ��ށB
		/// </summary>
		/// <param name="pBuffer">�g�`�f�[�^���������ރo�b�t�@�B</param>
		/// <param name="sizeToRead">�ǂݍ��ރf�[�^�T�C�Y�B</param>
		/// <param name="currentReadSize"></param>
		void Read(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/// <summary>
		/// �g�`�f�[�^��񓯊��ǂݍ��݁B
		/// </summary>
		/// <param name="pBuffer">�g�`�f�[�^���������ރo�b�t�@�B</param>
		/// <param name="sizeToRead">��݂��ރf�[�^�T�C�Y�B</param>
		/// <param name="currentReadSize"></param>
		void ReadAsync(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/// <summary>
		/// �ǂݍ��ݏI������B
		/// </summary>
		/// <returns>true�œǂݍ��ݏI���B</returns>
		bool IsReadEnd() const
		{
			return m_isReadEnd;
		}
		/// <summary>
		/// �g�`�f�[�^�̃V�[�N�ʒu�����Z�b�g�B
		/// </summary>
		void ResetFile();
		/// <summary>
		/// �J���B
		/// </summary>
		void Release();
		/// <summary>
		/// �t�@�C���T�C�Y���擾�B
		/// </summary>
		/// <returns>�t�@�C���T�C�Y�B</returns>
		DWORD   GetSize() const
		{
			return m_dwSize;
		}
		/// <summary>
		/// �t�@�C���t�H�[�}�b�g���擾�B
		/// </summary>
		/// <returns>�t�@�C���t�H�[�}�b�g�B</returns>
		WAVEFORMATEX* GetFormat() const
		{
			return m_pwfx;
		};
		/// <summary>
		/// �ǂݍ��ݐ�̃o�b�t�@���m�ہB
		/// </summary>
		/// <param name="size">�T�C�Y�B</param>
		void AllocReadBuffer(int size)
		{
			m_readBuffer.reset(new char[size]);
		}
		/// <summary>
		/// �ǂݍ��ݐ�̃o�b�t�@���擾�B
		/// </summary>
		/// <returns>�ǂݍ��ݐ�̃o�b�t�@�B</returns>
		char* GetReadBuffer()
		{
			return m_readBuffer.get();
		}
	private:
		std::unique_ptr<char[]>	m_readBuffer = nullptr;		//�ǂݍ��ݐ�̃o�b�t�@�B
		HMMIO					m_hmmio = nullptr;			//Wave�t�@�C���n���h���B
		WAVEFORMATEX* m_pwfx = nullptr;			//wave�t�@�C���̃t�H�[�}�b�g��`�B
		MMCKINFO 				m_ckRiff;					// Use in opening a WAVE file
		DWORD					m_dwSize = 0;				// The size of the wave file
		MMCKINFO				m_ck;						// Multimedia RIFF chunk
		BYTE* m_pbData = nullptr;
		BYTE* m_pbDataCur = nullptr;
		ULONG					m_ulDataSize = 0;
		volatile bool			m_isReadEnd = true;			//�ǂݍ��ݏI���t���O�B
		std::thread				m_readAsyncThread;			//�񓯊��ǂݍ��݃X���b�h�B
		bool					m_isInvokeReadAsyncThread = false;
		std::string				m_filePath;					//�t�@�C���p�X�B
	};
}