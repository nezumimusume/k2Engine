#include "k2EngineLowPreCompile.h"
#include "geometry/BVH.h"

namespace nsK2EngineLow {
	/// <summary>
	/// BVHを構築。
	/// </summary>
	/// <remark></remark>
	void BVH::Build()
	{
		// ルートノードを作成。
		m_rootNode = std::make_shared<SNode>();
		// ノードを内包するAABBを計算する。
		Vector3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		Vector3 vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
		for (auto& node : m_leafNodeArray) {
			vMax.Max(node->aabb.GetMax());
			vMin.Min(node->aabb.GetMin());
		}
		// ノードのAABBを初期化する。
		m_rootNode->aabb.Init(vMax, vMin);
		
		
	}
}