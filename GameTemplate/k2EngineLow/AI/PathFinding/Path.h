#pragma once

namespace nsK2EngineLow {
	namespace nsAI {
		
		class Path{
		public:
			/// <summary>
			/// パスの移動開始。
			/// </summary>
			void StartWalkPath()
			{
				m_sectionNo = 0;
			}
			/// <summary>
			/// パス移動。
			/// </summary>
			Vector3 WalkPath( Vector3 pos, float moveSpeed, bool& isEnd);
			/// <summary>
			/// クリア
			/// </summary>
			void Clear()
			{
				m_pointArray.clear();
				m_sectionArray.clear();
			}
			/// <summary>
			/// ポイントの追加。
			/// </summary>
			/// <param name="point"></param>
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

