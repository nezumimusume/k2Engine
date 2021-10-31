#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		class Path;
		class Cell;
		class NaviMesh;
		/// <summary>
		/// パス検索処理。
		/// </summary>
		class PathFinding
		{
		public:
			/// <summary>
			/// パスの検索処理を実行。
			/// </summary>
			/// <param name="path">見つかったパス</param>
			/// <param name="naviMesh">ナビゲーションメッシュ</param>
			/// <param name="startPos">検索開始座標</param>
			/// <param name="endPos">検索終了座標</param>
			void Execute(
				Path& path, 
				const NaviMesh& naviMesh, 
				const Vector3& startPos, 
				const Vector3& endPos
			);
		private:
			struct CellWork {
			
				void Init(const Cell* cell)
				{
					this->cell = cell;
					parentCell = nullptr;
					costFromStartCell = 0.0f;
					cost = FLT_MAX;
					isOpend = false;
					isClosed = false;
				}
				const Cell* cell;
				CellWork* parentCell;	// 親のセル。
				float costFromStartCell;
				float cost;		// 移動コスト
				bool isOpend;	// 開かれた？
				bool isClosed;	// 閉じられた？
			};

			std::vector< CellWork > m_cellWork;
		};
	}
}

