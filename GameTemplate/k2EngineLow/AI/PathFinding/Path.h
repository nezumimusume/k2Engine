#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		class NaviMesh;
		/// <summary>
		/// パスを扱うクラス。
		/// </summary>
		class Path{
		public:
			/// <summary>
			/// パス上を移動する。
			/// </summary>
			/// <param name="pos">移動させる座標</param>
			/// <param name="moveSpeed">移動速度</param>
			/// <param name="isEnd">パス移動終了したらtrueが設定される</param>
			/// <param name="physicsWorld">
			/// 物理ワールド。
			/// 物理ワールドが指定されていると、パス移動後に地面にレイキャストを行い、
			/// 座標を地面にスナップします。
			/// </param>
			/// <returns>移動後の座標</returns>
			Vector3 Move( 
				Vector3 pos, 
				float moveSpeed, 
				bool& isEnd,
				PhysicsWorld* physicsWorld = nullptr
			);
			/// <summary>
			/// クリア
			/// </summary>
			void Clear()
			{
				m_sectionNo = 0;
				m_pointArray.clear();
				m_sectionArray.clear();
			}
			/// <summary>
			/// ポイントの追加。
			/// </summary>
			/// <param name="point">ポイント</param>
			void AddPoint(const Vector3& point)
			{
				m_pointArray.push_back(point);
			}
			/// <summary>
			/// パスを構築。
			/// </summary>
			void Build();
		private:
			/// <summary>
			/// パスのセクション
			/// </summary>
			struct SSection {
				Vector3 startPos;	// セクションの開始座標。
				Vector3 endPos;		// セクションの終了座標。
				Vector3 direction;	// セクションの方向。
				float length;		// セクションの長さ。
			};
			std::vector<Vector3>	m_pointArray;	// ポイントの配列
			std::vector< SSection >	m_sectionArray;	// セクションの配列。
			int m_sectionNo = 0;					// セクション番号。
		};
	}
}

