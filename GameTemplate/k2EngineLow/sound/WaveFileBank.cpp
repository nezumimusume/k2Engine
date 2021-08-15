/*!
 *@brief	波形データバンク。
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
		//波形データを開放する。
		Release();
	}

	void WaveFileBank::Resist(int number, const char* filePath)
	{
		//既に波形データが登録されていたら。
		if (m_waveFilePtrArray[number])
		{
			//何もしない。
			return;
		}
		m_waveFilePtrArray[number].reset(new WaveFile);
		auto waveFile = m_waveFilePtrArray[number];
		bool result = waveFile->Open(filePath);
		//ファイルオープンに失敗したら。
		if (result == false) {
			//リソースを破棄する。
			m_waveFilePtrArray[number].reset();
			return;
		}
		waveFile->AllocReadBuffer(waveFile->GetSize());	//waveファイルのサイズ分の読み込みバッファを確保する。
		unsigned int dummy;
		waveFile->Read(waveFile->GetReadBuffer(), waveFile->GetSize(), &dummy);
		waveFile->ResetFile();
	}
}