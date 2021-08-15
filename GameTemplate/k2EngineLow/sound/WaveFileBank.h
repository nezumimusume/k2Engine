/// <summary>
/// �g�`�f�[�^�o���N�B
/// </summary>
#pragma once
#include <array>

namespace nsK2EngineLow {
	//�o�^�ł���g�`�f�[�^�̌��E���B
	const int MAXWAVEFILENUMBER = 1000;

	class WaveFile;
	typedef std::shared_ptr<WaveFile>	WaveFilePtr;
	/// <summary>
	/// �g�`�f�[�^�o���N�B
	/// WaveFile::Resist�Ŕg�`�f�[�^�����[�h���āA�o���N�ɓo�^���邱�Ƃ��ł��܂��B
	/// �o���N�ɓo�^���ꂽ�g�`�f�[�^�́ASoundSource::Init�ŗ��p���܂��B
	/// </summary>
	class WaveFileBank : public Noncopyable {
	public:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		WaveFileBank();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~WaveFileBank();
		/// <summary>
		/// �g�`�f�[�^���J������B
		/// </summary>
		void Release()
		{
			for (int i = 0; i < m_waveFilePtrArray.size(); i++)
			{
				m_waveFilePtrArray[i].reset();
			}
		}
		/// <summary>
		/// �g�`�f�[�^����������B
		/// </summary>
		/// <param name="number">WaveFile::Resist�œo�^�����ԍ��B</param>
		/// <returns></returns>
		WaveFilePtr FindWaveFile(int number)
		{
			return m_waveFilePtrArray[number];
		}
		/// <summary>
		/// �g�`�f�[�^��o�^����B
		/// </summary>
		/// <param name="number">�o�^����ԍ��B</param>
		/// <param name="filePath">wave�t�@�C���̃t�@�C���p�X�B</param>
		void Resist(int number, const char* filePath);
	private:
		std::array<WaveFilePtr, MAXWAVEFILENUMBER> m_waveFilePtrArray;		//�g�`�f�[�^�̔z��B
	};
}