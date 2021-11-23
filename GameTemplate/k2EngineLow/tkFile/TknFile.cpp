#include "k2EngineLowPreCompile.h"
#include "tkFile/TknFile.h"

namespace nsK2EngineLow {
	bool TknFile::Load(const char* filePath)
	{
		auto fp = fopen(filePath, "rb");
		if (fp == nullptr) {
			// ファイルのオープンに失敗した。
			return false;
		}
		// ヘッダをロード。
		fread(&m_header, sizeof(m_header), 1, fp);

		// セルの配列を確保
		m_cellArray.resize(m_header.numCell);

		// セル情報をロード。
		fread(&m_cellArray.front(), sizeof(SCell) * m_header.numCell, 1, fp);

		// ボディをロード。
		fclose(fp);
		return true;
	}
}