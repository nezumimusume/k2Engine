/// <summary>
/// waveファイル。
/// </summary>
#pragma once

#include <thread>

namespace nsK2EngineLow {
	/// <summary>
	/// Waveファイルを扱うためのクラス。
	/// Read()で同期読み込み、ReadAsync()は非同期読み込みが行われます。
	/// ReadAsyncを利用した場合はWaveFile::IsReadEndを利用して同期をとってください。
	/// </summary>
	class WaveFile : public Noncopyable {
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		WaveFile();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~WaveFile();
		/// <summary>
		/// waveファイルをオープン。
		/// </summary>
		/// <param name="fileName">ファイル名。</param>
		/// <returns>falseを返して来たらオープンに失敗。</returns>
		bool Open(const char* fileName);
		/// <summary>
		/// 波形データを読み込む。
		/// </summary>
		/// <param name="pBuffer">波形データを書き込むバッファ。</param>
		/// <param name="sizeToRead">読み込むデータサイズ。</param>
		/// <param name="currentReadSize"></param>
		void Read(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/// <summary>
		/// 波形データを非同期読み込み。
		/// </summary>
		/// <param name="pBuffer">波形データを書き込むバッファ。</param>
		/// <param name="sizeToRead">よみこむデータサイズ。</param>
		/// <param name="currentReadSize"></param>
		void ReadAsync(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/// <summary>
		/// 読み込み終了判定。
		/// </summary>
		/// <returns>trueで読み込み終了。</returns>
		bool IsReadEnd() const
		{
			return m_isReadEnd;
		}
		/// <summary>
		/// 波形データのシーク位置をリセット。
		/// </summary>
		void ResetFile();
		/// <summary>
		/// 開放。
		/// </summary>
		void Release();
		/// <summary>
		/// ファイルサイズを取得。
		/// </summary>
		/// <returns>ファイルサイズ。</returns>
		DWORD   GetSize() const
		{
			return m_dwSize;
		}
		/// <summary>
		/// ファイルフォーマットを取得。
		/// </summary>
		/// <returns>ファイルフォーマット。</returns>
		WAVEFORMATEX* GetFormat() const
		{
			return m_pwfx;
		};
		/// <summary>
		/// 読み込み先のバッファを確保。
		/// </summary>
		/// <param name="size">サイズ。</param>
		void AllocReadBuffer(int size)
		{
			m_readBuffer.reset(new char[size]);
		}
		/// <summary>
		/// 読み込み先のバッファを取得。
		/// </summary>
		/// <returns>読み込み先のバッファ。</returns>
		char* GetReadBuffer()
		{
			return m_readBuffer.get();
		}
	private:
		std::unique_ptr<char[]>	m_readBuffer = nullptr;		//読み込み先のバッファ。
		HMMIO					m_hmmio = nullptr;			//Waveファイルハンドル。
		WAVEFORMATEX* m_pwfx = nullptr;			//waveファイルのフォーマット定義。
		MMCKINFO 				m_ckRiff;					// Use in opening a WAVE file
		DWORD					m_dwSize = 0;				// The size of the wave file
		MMCKINFO				m_ck;						// Multimedia RIFF chunk
		BYTE* m_pbData = nullptr;
		BYTE* m_pbDataCur = nullptr;
		ULONG					m_ulDataSize = 0;
		volatile bool			m_isReadEnd = true;			//読み込み終了フラグ。
		std::thread				m_readAsyncThread;			//非同期読み込みスレッド。
		bool					m_isInvokeReadAsyncThread = false;
		std::string				m_filePath;					//ファイルパス。
	};
}