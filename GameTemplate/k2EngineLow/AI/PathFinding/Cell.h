#pragma once

namespace nsK2EngineLow {
	class TknFile;
	namespace nsAI {
		/// <summary>
		/// ナビメッシュのセル
		/// </summary>
		class Cell {
		public:
			/// <summary>
			/// 頂点座標を設定。
			/// </summary>
			/// <param name="vertNo">頂点番号</param>
			/// <param name="pos">座標</param>
			void SetVertexPosition(int vertNo, const Vector3& pos)
			{
				K2_ASSERT(vertNo < 3, "Invalid vertNo");
				m_vertexPosition[vertNo] = pos;
			}
			/// <summary>
			/// 法線を設定。
			/// </summary>
			/// <param name="normal">法線</param>
			void SetNormal(const Vector3& normal)
			{
				m_normal = normal;
			}
			/// <summary>
			/// 隣接セル情報を設定。
			/// </summary>
			/// <param name="linkNo">隣接番号。0～2。</param>
			/// <param name="linkCell">隣接セル</param>
			void SetLinkCell(int linkNo, Cell* linkCell)
			{
				K2_ASSERT(linkNo < 3, "Invalid linkNo");
				m_linkCell[linkNo] = linkCell;
			}
			/// <summary>
			/// セルの中心座標を設定。
			/// </summary>
			/// <param name="centerPos"></param>
			void SetCenterPosition(const Vector3& centerPos)
			{
				m_centerPos = centerPos;
			}
			/// <summary>
			/// セルの中心座標を取得。
			/// </summary>
			/// <returns></returns>
			const Vector3& GetCenterPosition()const
			{
				return m_centerPos;
			}
			/// <summary>
			/// セル番号を設定。
			/// </summary>
			/// <param name="no">番号</param>
			void SetCellNo(int no)
			{
				m_no = no;
			}
			/// <summary>
			/// セル番号を取得。
			/// </summary>
			/// <returns></returns>
			int GetCellNo() const
			{
				return m_no;
			}
			/// <summary>
			/// 隣接セルを取得。
			/// </summary>
			/// <param name="linkCellNo"></param>
			/// <returns></returns>
			Cell* GetLinkCell(int linkCellNo)const
			{
				return m_linkCell[linkCellNo];
			}
			/// <summary>
			/// セルの頂点座標を取得する。
			/// </summary>
			/// <param name="vertNo">頂点番号</param>
			/// <returns>頂点座標</returns>
			const Vector3& GetVertexPosition(int vertNo) const
			{
				return m_vertexPosition[vertNo];
			}
		private:
			Vector3 m_vertexPosition[3];		// セルの頂点座標
			Vector3 m_normal;					// 法線
			Vector3 m_centerPos;				// 中心座標。
			Cell* m_linkCell[3] = { nullptr };	// 隣接セル
			int m_no = 0;						// セル番号
		};
	}
}

