/// <summary>
/// 波形データバンク。
/// </summary>
#pragma once
#include <array>

namespace nsK2EngineLow {
	//登録できる波形データの限界数。
	const int MAXWAVEFILENUMBER = 1000;

	class WaveFile;
	typedef std::shared_ptr<WaveFile>	WaveFilePtr;
	/// <summary>
	/// 波形データバンク。
	/// WaveFile::Resistで波形データをロードして、バンクに登録することができます。
	/// バンクに登録された波形データは、SoundSource::Initで利用します。
	/// </summary>
	class WaveFileBank : public Noncopyable {
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		WaveFileBank();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~WaveFileBank();
		/// <summary>
		/// 波形データを開放する。
		/// </summary>
		void Release()
		{
			for (int i = 0; i < m_waveFilePtrArray.size(); i++)
			{
				m_waveFilePtrArray[i].reset();
			}
		}
		/// <summary>
		/// 波形データを検索する。
		/// </summary>
		/// <param name="number">WaveFile::Resistで登録した番号。</param>
		/// <returns></returns>
		WaveFilePtr FindWaveFile(int number)
		{
			return m_waveFilePtrArray[number];
		}
		/// <summary>
		/// 波形データを登録する。
		/// </summary>
		/// <param name="number">登録する番号。</param>
		/// <param name="filePath">waveファイルのファイルパス。</param>
		void Resist(int number, const char* filePath);
	private:
		std::array<WaveFilePtr, MAXWAVEFILENUMBER> m_waveFilePtrArray;		//波形データの配列。
	};
}