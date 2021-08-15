/*!
 * @brief	waveファイル。
 */

#include "k2EngineLowPreCompile.h"
#include "WaveFile.h"


namespace nsK2EngineLow {

	WaveFile::WaveFile()
	{
	}

	WaveFile::~WaveFile()
	{
		Release();
	}

	bool WaveFile::Open(const char* fileName)
	{
		m_filePath = fileName;
		m_hmmio = mmioOpenA(const_cast<char*>(fileName), NULL, MMIO_ALLOCBUF | MMIO_READ);
		if (m_hmmio == NULL) {
			//TK_WARNING_MESSAGE_BOX("waveファイルのオープンに失敗しました。filePath : %s\n"
							/*"原因として下記の２点が考えられます。\n"
							"①　ファイルパスが間違っている。\n"
							"②　ファイルがAssetsフォルダの中にない。\n"
							"上記２点を確認して、問題がない場合は一度VisualStudioのビルド/リビルドを行ってみてください。\n", fileName);*/
							//TK_WARNING("Failed mmioOpen");
			return false;
		}
		MMCKINFO ckIn;           // chunk info. for general use.
		PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.
		memset(&ckIn, 0, sizeof(ckIn));

		m_pwfx = NULL;

		if ((0 != mmioDescend(m_hmmio, &m_ckRiff, NULL, 0))) {
			//TK_WARNING_MESSAGE_BOX("Failed mmioDescend");
			Release();
			return false;
		}
		if ((m_ckRiff.ckid != FOURCC_RIFF) ||
			(m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E'))) {
			//TK_WARNING_MESSAGE_BOX("Failed mmioDescend");
			Release();
			return false;
		}

		// Search the input file for for the 'fmt ' chunk.
		ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if (0 != mmioDescend(m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK)) {
			//TK_WARNING_MESSAGE_BOX("Failed mmioDescend");
			Release();
			return false;
		}

		// Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
		// if there are extra parameters at the end, we'll ignore them
		if (ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT)) {
			//TK_WARNING_MESSAGE_BOX("Failed mmioDescend");
			Release();
			return false;
		}

		// Read the 'fmt ' chunk into <pcmWaveFormat>.
		if (mmioRead(m_hmmio, (HPSTR)&pcmWaveFormat,
			sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat)) {
			//TK_WARNING_MESSAGE_BOX("Failed mmioRead");
			Release();
			return false;
		}

		// Allocate the waveformatex, but if its not pcm format, read the next
		// word, and thats how many extra bytes to allocate.
		if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
		{
			m_pwfx = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX)];

			// Copy the bytes from the pcm structure to the waveformatex structure
			memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
			m_pwfx->cbSize = 0;
		}
		else
		{
			// Read in length of extra bytes.
			WORD cbExtraBytes = 0L;
			if (mmioRead(m_hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD)) {
				//TK_WARNING_MESSAGE_BOX("Failed mmioRead");
				Release();
				return false;
			}

			m_pwfx = (WAVEFORMATEX*)new CHAR[sizeof(WAVEFORMATEX) + cbExtraBytes];

			// Copy the bytes from the pcm structure to the waveformatex structure
			memcpy(m_pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat));
			m_pwfx->cbSize = cbExtraBytes;

			// Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
			if (mmioRead(m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx->cbSize)) + sizeof(WORD)),
				cbExtraBytes) != cbExtraBytes)
			{
				//TK_WARNING_MESSAGE_BOX("Failed mmioRead");
				Release();
				return false;
			}
		}

		// Ascend the input file out of the 'fmt ' chunk.
		if (0 != mmioAscend(m_hmmio, &ckIn, 0))
		{
			//TK_WARNING_MESSAGE_BOX("Failed mmioAscend");
			Release();
			return false;
		}
		ResetFile();
		m_dwSize = m_ck.cksize;
		return true;
	}
	void WaveFile::ResetFile()
	{
		while (!m_isReadEnd) {}	//読み込み中にリセットはさせない。
		if (m_hmmio == NULL)
			return;

		// Seek to the data
		if (-1 == mmioSeek(m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC),
			SEEK_SET)) {
			//TK_WARNING("Failed mmioSeek");
			return;
		}

		// Search the input file for the 'data' chunk.
		m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if (0 != mmioDescend(m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK)) {
			//TK_WARNING("Failed mmioDescend");
			return;
		}
	}

	void WaveFile::Read(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize)
	{
		MMIOINFO mmioinfoIn; // current status of m_hmmio

		if (m_hmmio == NULL) {
			return;
		}
		if (pBuffer == NULL) {
			return;
		}


		if (0 != mmioGetInfo(m_hmmio, &mmioinfoIn, 0)) {
			//TK_WARNING("mmioGetInfo");
			return;
		}

		UINT cbDataIn = sizeToRead;
		if (cbDataIn > m_ck.cksize) {
			cbDataIn = m_ck.cksize;
		}
		*currentReadSize = cbDataIn;
		m_ck.cksize -= cbDataIn;

		for (DWORD cT = 0; cT < cbDataIn; cT++)
		{
			// Copy the bytes from the io to the buffer.
			if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
			{
				if (0 != mmioAdvance(m_hmmio, &mmioinfoIn, MMIO_READ)) {
					//TK_WARNING("mmioAdvance");
					return;
				}

				if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead) {
					//TK_WARNING("mmioinfoIn.pchNext");
					return;
				}
			}

			// Actual copy.
			*((BYTE*)pBuffer + cT) = *((BYTE*)mmioinfoIn.pchNext);
			mmioinfoIn.pchNext++;
		}

		/*if (0 != mmioSetInfo(m_hmmio, &mmioinfoIn, 0)) {
			//TK_LOG("mmioSetInfo");
			return;
		}*/
		m_isReadEnd = true;
		return;
	}
	void WaveFile::ReadAsync(char* pBuffer, unsigned int sizeToRead, unsigned int* currentReadSize)
	{
		if (m_isInvokeReadAsyncThread) {
			m_readAsyncThread.join();
			m_isInvokeReadAsyncThread = false;
		}
		m_isReadEnd = false;

		//読み込みスレッドを立てる。
		m_readAsyncThread = std::thread([this, pBuffer, sizeToRead, currentReadSize] {
			this->Read(pBuffer, sizeToRead, currentReadSize);
		});
		m_isInvokeReadAsyncThread = true;
	}
	void WaveFile::Release()
	{
		if (m_isInvokeReadAsyncThread) {
			m_readAsyncThread.join();
			m_isInvokeReadAsyncThread = false;
		}

		if (m_hmmio != NULL)
		{
			mmioClose(m_hmmio, 0);
			m_hmmio = NULL;
		}
		delete[] m_pwfx;
		m_pwfx = NULL;
	}
}