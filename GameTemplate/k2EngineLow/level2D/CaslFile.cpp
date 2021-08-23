#include "k2EngineLowPreCompile.h"
#include "CaslFile.h"
#include <string>

namespace nsK2EngineLow {
	void CaslFile::Load(const char* filePath)
	{
		auto fp = fopen(filePath, "rb");
		//ファイル読み込めなかったら。
		if (fp == nullptr) {
			MessageBoxA(nullptr, "caslファイルのオープンに失敗しました。", "エラー", MB_OK);
			return;
		}

		//.ddsフォルダパスを設定。。
		std::string ddsFolderPath = filePath;
		int pos = (int)ddsFolderPath.rfind("/");
		ddsFolderPath = ddsFolderPath.substr(0, static_cast<size_t>(pos) + 1);
		int ddsFolderPathCount = static_cast<int>(ddsFolderPath.length());

		//画像の数を取得。
		int numLevel = ReadInteger(fp);
		for (int i = 0; i < numLevel; i++)
		{

			auto caslData = std::make_unique<CaslData>();
			//std::unique_ptr<Level2DObject> levelObject;
			//レベルファイルに保存してあるのは。
			//以下全てchar型で保存してある、一つ一つに(,)で区切りがしてある。
			//名前の長さ(int)、名前(char)。
			//ファイルパスの長さ(int)、ファイルパス(char)。
			//座標x,y(float)。
			//画像の大きさx,y(int)。
			//レイヤー優先度x,y(int)。
			//スケール倍率x,y(float)。
			//.ddsファイルの名前の長さ(int)、ddsファイルの名前。

			//画像の名前の長さを取得。
			int nameCount = ReadInteger(fp);
			//画像の名前を取得。
			caslData.get()->name = std::make_unique<char[]>(static_cast<size_t>(nameCount) + 1);
			fread(caslData.get()->name.get(), nameCount, 1, fp);
			ReadOnlyOneCharacter(fp);

			//ファイルパスの名前の長さを取得。
			int fileNameCount = ReadInteger(fp);

			//ファイルパスを取得。
			caslData.get()->fileName = std::make_unique<char[]>(static_cast<size_t>(fileNameCount) + 1);
			fread(caslData.get()->fileName.get(), fileNameCount, 1, fp);
			ReadOnlyOneCharacter(fp);

			//座標を取得。
			caslData.get()->position.x = ReadDecimal(fp);
			caslData.get()->position.y = ReadDecimal(fp);

			//幅と高さを取得。
			caslData.get()->width = ReadInteger(fp);
			caslData.get()->height = ReadInteger(fp);

			//レイヤー優先度を取得。
			caslData.get()->numberLayer = ReadInteger(fp);

			//大きさの倍率を取得。
			//現在は1.0fに固定。
			ReadDecimal(fp);
			ReadDecimal(fp);
			caslData.get()->scale.x = 1.0f;
			caslData.get()->scale.y = 1.0f;

			//ピボットを取得。
			caslData.get()->pivot.x = ReadDecimal(fp);
			caslData.get()->pivot.y = ReadDecimal(fp);

			//ddsファイルパスの名前の長さを取得。
			int ddsFileNameCount = ReadInteger(fp);
			//ddsファイルパスを取得。
			caslData.get()->ddsFileName = std::make_unique<char[]>(static_cast<size_t>(ddsFileNameCount) + 1);
			fread(caslData.get()->ddsFileName.get(), ddsFileNameCount, 1, fp);

			//.ddsファイルパスを設定する。
			caslData.get()->ddsFilePath = std::make_unique<char[]>(static_cast<size_t>(ddsFolderPathCount) + static_cast<size_t>(ddsFileNameCount) + 1);
			//フォルダパスと.ddsファイルの名前を連結する。
			std::string ddsFilePath = ddsFolderPath + caslData.get()->ddsFileName.get();
			std::char_traits<char>::copy(caslData.get()->ddsFilePath.get(), ddsFilePath.c_str(), ddsFilePath.size() + 1);

			//ReadOnlyOneCharacter(fp);
			//改行コードを読み飛ばし。
			char dummy[256];
			fgets(dummy, 256, fp);

			m_caslDataList.push_back(std::move(caslData));
		}
		//fopenしたらfcloseで閉じる。
		fclose(fp);
	}

	void CaslFile::ReadOnlyOneCharacter(FILE* file) const
	{
		char a;
		fread(&a, 1, 1, file);
	}

	int CaslFile::ReadInteger(FILE* file) const
	{
		std::string number;
		while (true)
		{
			char c;
			fread(&c, 1, 1, file);
			if (c != ',' && c != '\n')
			{
				number += c;
			}
			else {
				break;
			}
		}
		//string型をint型に変換する。
		return atoi(number.c_str());
	}

	float CaslFile::ReadDecimal(FILE* file) const
	{
		std::string number;
		while (true)
		{
			char c;
			fread(&c, 1, 1, file);
			if (c != ',' && c != '\n')
			{
				number += c;
			}
			else {
				break;
			}
		}
		//string型をfloat型に変換する。
		return static_cast<float>(atof(number.c_str()));
	}
}