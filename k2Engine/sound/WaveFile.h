/*!
 * @brief	waveファイル
 */

#pragma once

#include <thread>


	/*!
	 *@brief	waveファイルを扱うためのクラス。
	 *@details
	 * waveファイルの同期読み込み、非同期読み込みをサポートします。</br>
	 * WaveFile::Readは同期読み込み、WaveFile::ReadAsyncは非同期読み込みが行われます。</br>
	 * ReadAsyncを使用した場合はWaveFile::IsReadEndを使用して同期をとって下さい。</br>
	 */
	class WaveFile {
	public:
		WaveFile();
		~WaveFile();
		/*!
		 * @brief	waveファイルをオープン。
		 *@param[in]	fileName		ファイル名。
		 *@return	falseを返して来たらオープンに失敗。
		 */
		bool Open( const char* fileName );
		/*!
		* @brief	波形データを読み込み。
		*@param[out]	pBuffer		波形データを書き込むバッファ。
		*@param[in]		sizeToRead	読み込むデータサイズ。
		*/
		void Read(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/*!
		* @brief	波形データを非同期読み込み。
		*@param[out]	pBuffer		波形データを書き込むバッファ。
		*@param[in]		sizeToRead	読み込むデータサイズ。
		*/
		void ReadAsync( char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize);
		/*!
		* @brief	読み込み終了判定。
		*/
		bool IsReadEnd() const
		{
			return m_isReadEnd;
		}
		/*!
		* @brief	波形データのシーク位置をリセット。
		*/
		void ResetFile();
		/*!
		 * @brief	リリース。
		 */
		void Release();
		/*!
		* @brief	ファイルサイズを取得。
		*/
		DWORD   GetSize() const
		{
			return m_dwSize;
		}
		/*!
		* @brief	ファイルフォーマットを取得。
		*/
		WAVEFORMATEX* GetFormat() const
		{
			return m_pwfx;
		};
		/*!
		* @brief	読み込み先のバッファを確保。
		*/
		void AllocReadBuffer(int size)
		{
			m_readBuffer.reset(new char[size]);
		}
		/*!
		* @brief	読み込み先のバッファを取得。
		*/
		char* GetReadBuffer()
		{
			return m_readBuffer.get();
		}
	private:
		std::unique_ptr<char[]>	m_readBuffer = nullptr;	//読み込み先のバッファ。
		HMMIO			m_hmmio = nullptr;	//Waveファイルハンドル。
		WAVEFORMATEX* 	m_pwfx = nullptr;  //waveファイルのフォーマット定義。
		MMCKINFO 		m_ckRiff;      // Use in opening a WAVE file
		DWORD			m_dwSize = 0;      // The size of the wave file
		MMCKINFO		m_ck;          // Multimedia RIFF chunk
		BYTE*			m_pbData = nullptr;
		BYTE*			m_pbDataCur = nullptr;
		ULONG			m_ulDataSize = 0;
		volatile bool	m_isReadEnd = true;	//読み込み終了フラグ。
		std::thread		m_readAsyncThread;	//非同期読み込みスレッド。
		bool			m_isInvokeReadAsyncThread = false;
		std::string	m_filePath;			//ファイルパス。
	};