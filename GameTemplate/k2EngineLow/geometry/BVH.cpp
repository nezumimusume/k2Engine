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
		
		// 主成分分析を行って、分割平面を求める。
		// まずは、AABBの中心座標を求める。
		Vector3 centerPos;
		for (auto& node : m_leafNodeArray) {
			centerPos += node->aabb.GetCenter();
		}
		centerPos /= m_leafNodeArray.size();
		// 続いて共分散行列を計算する
		Matrix covarianceMatrix;
		for (auto& node : m_leafNodeArray) {
			covarianceMatrix[0][0] += node->aabb.GetCenter();
		}
	}
}