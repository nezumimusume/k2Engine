#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// tknファイル
	/// </summary>
	class TknFile : public Noncopyable {
	public:
		/// <summary>
		/// tknファイルのヘッダー
		/// </summary>
		struct SHeader {
			std::uint32_t numCell;	// セルの数。
		};
		/// <summary>
		/// セルのデータ。
		/// </summary>
		struct SCell {
			Vector3 vertexPosition[3];	// セルの頂点座標。
			Vector3 normal;				// セルの法線。
			std::int32_t linkCellNo[3];	// 隣接セルの番号。
		};
		/// <summary>
		/// tknファイルをロードする。
		/// </summary>
		/// <param name="filePath">tknファイルのファイルパス</param>
		/// <returns>falseが返ってきたらロード失敗。</returns>
		bool Load(const char* filePath);
		/// <summary>
		/// セルの数を取得。
		/// </summary>
		/// <returns></returns>
		int GetNumCell() const
		{
			return m_header.numCell;
		}
		/// <summary>
		/// セルを取得
		/// </summary>
		/// <param name="cellNo">セル番号</param>
		/// <returns></returns>
		const SCell& GetCell(int cellNo) const
		{
			return m_cellArray.at(cellNo);
		}
	private:
		SHeader m_header;				// ヘッダ。
		std::vector<SCell> m_cellArray;	// セルの配列。
	};
}