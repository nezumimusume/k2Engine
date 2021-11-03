#pragma once

#include "AI/PathFinding/Cell.h"
#include "geometry/BSP.h"

namespace nsK2EngineLow {
	class TknFile;
	namespace nsAI {
		class Cell;
		/// <summary>
		/// ナビゲーションメッシュクラス。
		/// </summary>
		/// <remark>
		/// 3dsMaxからnvmExporterを使用して出力されたデータをもとに構築されます。
		/// </remark>
		class NaviMesh{
		public:
			/// <summary>
			/// tknファイルからナビゲーションメッシュを構築する。
			/// </summary>
			/// <param name="tknFilePath">tknファイルのファイルパス</param>
			void Init(const char* tknFilePath);
			/// <summary>
			/// 指定した座標に最も近いセルを検索する。
			/// </summary>
			/// <param name="pos">座標</param>
			/// <returns>セル</returns>
			const Cell& FindNearestCell(const Vector3& pos) const;
			/// <summary>
			/// セルの数を取得。
			/// </summary>
			/// <returns></returns>
			int GetNumCell() const
			{
				return static_cast<int>(m_cellArray.size());
			}
			/// <summary>
			/// セルを取得。
			/// </summary>
			/// <param name="cellNo"></param>
			/// <returns></returns>
			const Cell& GetCell(int cellNo) const
			{
				return m_cellArray.at(cellNo);
			}
		private:
			std::vector< Cell > m_cellArray;	// セルの配列
			BSP m_cellCenterPosBSP;				// セルの中心座標で構築されたBSP
		};
	}
}

