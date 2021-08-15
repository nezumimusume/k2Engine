/*!
 *@brief	�g�`�f�[�^�o���N�B
 */

#include "k2EngineLowPreCompile.h"
#include "WaveFile.h"
#include "WaveFileBank.h"

namespace nsK2EngineLow {
	WaveFileBank::WaveFileBank()
	{
	}
	WaveFileBank::~WaveFileBank()
	{
		//�g�`�f�[�^���J������B
		Release();
	}

	void WaveFileBank::Resist(int number, const char* filePath)
	{
		//���ɔg�`�f�[�^���o�^����Ă�����B
		if (m_waveFilePtrArray[number])
		{
			//�������Ȃ��B
			return;
		}
		m_waveFilePtrArray[number].reset(new WaveFile);
		auto waveFile = m_waveFilePtrArray[number];
		bool result = waveFile->Open(filePath);
		//�t�@�C���I�[�v���Ɏ��s������B
		if (result == false) {
			//���\�[�X��j������B
			m_waveFilePtrArray[number].reset();
			return;
		}
		waveFile->AllocReadBuffer(waveFile->GetSize());	//wave�t�@�C���̃T�C�Y���̓ǂݍ��݃o�b�t�@���m�ۂ���B
		unsigned int dummy;
		waveFile->Read(waveFile->GetReadBuffer(), waveFile->GetSize(), &dummy);
		waveFile->ResetFile();
	}
}