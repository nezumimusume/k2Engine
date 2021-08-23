#pragma once
#include <list>
#include <vector>

namespace nsK2EngineLow {
	/// <summary>
	/// Caslファイルのデータ。
	/// </summary>
	struct CaslData : public Noncopyable
	{
	public:
		std::unique_ptr<char[]> name;			//名前。
		std::unique_ptr<char[]> fileName;		//ファイルのパス。
		std::unique_ptr<char[]> ddsFileName;	//ddsファイル。
		std::unique_ptr<char[]> ddsFilePath;	//ddsファイルのパス。
		Vector2 position;						//座標。
		int width = 0;							//横幅。
		int height = 0;							//縦幅。
		int numberLayer = 0;					//レイヤー優先度。
		Vector2 scale;							//大きさ。
		Vector2 pivot = Sprite::DEFAULT_PIVOT;	//ピボット。
	};

	/// <summary>
	/// caslファイルを読み込む。
	/// </summary>
	class CaslFile
	{
	public:
		/// <summary>
		/// caslファイルを読み込む。
		/// </summary>
		/// <param name="filePath">ファイルパス。</param>
		void Load(const char* filePath);
		/// <summary>
		/// Caslデータの数を取得。
		/// </summary>
		/// <returns>Caslデータの数。</returns>
		int GetNumCaslData() const
		{
			return static_cast<int>(m_caslDataList.size());
		}
		/// <summary>
		/// Caslデータを取得。
		/// </summary>
		/// <param name="number">ナンバー。</param>
		/// <returns></returns>
		CaslData* GetCaslData(int number) const
		{
			return m_caslDataList[number].get();
		}
	private:
		//1文字だけ読み込む、(,)を読み込むのに使う。
		/// <summary>
		/// 1文字だけ読み込む。「 , 」や「 \n 」を読み込むのに使う。
		/// </summary>
		/// <param name="file">ファイル。</param>
		void ReadOnlyOneCharacter(FILE* file) const;
		/// <summary>
		/// int型を読み込む。
		/// </summary>
		/// <param name="file">ファイル。</param>
		/// <returns>int型の値。</returns>
		int ReadInteger(FILE* file) const;
		/// <summary>
		/// float型を読み込む。
		/// </summary>
		/// <param name="file">ファイル。</param>
		/// <returns>float型の値。</returns>
		float ReadDecimal(FILE* file) const;

		std::vector<std::unique_ptr<CaslData>> m_caslDataList;		//Caslデータ。
	};
}
