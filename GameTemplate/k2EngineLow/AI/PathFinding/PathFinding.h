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
			/// <param name="physicsWorld">
			/// 物理ワールド。
			/// 物理ワールドが指定されていると、物理ワールドに配置されているオブジェクトと
			/// 衝突判定を行い、パスのスムージングが改善します。
			/// この衝突判定では、agentRadiusとagentHeightのパラメータが使用されます。
			/// </param>
			/// <param name="agentRadius">
			/// AIエージェントの半径。エージェントのカプセルコライダーを作成するのに利用されます。
			/// </param>
			/// <param name="agentHeight">
			/// AIエージェントの高さエージェントのカプセルコライダーを作成するのに利用されます。
			/// </param>
			void Execute(
				Path& path, 
				const NaviMesh& naviMesh, 
				const Vector3& startPos, 
				const Vector3& endPos,
				PhysicsWorld* physicsWorld = nullptr,
				float agentRadius = 50.0f,
				float agentHeight = 200.0f
			);
		private:
			struct CellWork {

				void Init(const Cell* cell);
				
				const Cell* cell;
				CellWork* parentCell;	// 親のセル。
				float costFromStartCell;
				Vector3 pathPoint;
				float cost;				// 移動コスト
				bool isOpend;			// 開かれた？
				bool isClosed;			// 閉じられた？
				bool isSmooth;			// スムースされる？
			};
		private:
		
			/// <summary>
			/// 次のセルに移動するコストを計算
			/// </summary>
			/// <returns></returns>
			void CalcCost(
				float& totalCost, 
				float& costFromStartCell, 
				const CellWork* nextCell, 
				const CellWork* prevCell, 
				const Cell* endCell);
			/// <summary>
			/// スムージング
			/// </summary>
			/// <param name="cellList"></param>
			void Smoothing(
				std::list<CellWork*>& cellList,
				PhysicsWorld* physicsWorld,
				float agentRadius,
				float agentHeight
			);
			/// <summary>
			/// レイとセルの交差判定
			/// </summary>
			bool IsIntercetRayToCell(Vector3 rayStartPos, Vector3 rayEndPos, CellWork*) const;
		private:
			

			std::vector< CellWork > m_cellWork;
		};
	}
}

