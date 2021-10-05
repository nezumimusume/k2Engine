
#pragma once

#include "geometry/AABB.h"

namespace nsK2EngineLow {

	/// <summary>
	/// BVH
	/// </summary>
	/// <remark></remark>
	class BVH {
	public:
		/// <summary>
		/// AABBを追加
		/// </summary>
		/// <param name="aabb"></param>
		void AddLeaf(const AABB& aabb) 
		{
			// リーフノードを作る。
			auto leafNode = std::make_shared<SNode>();
			leafNode->aabb = aabb;
			m_leafNodeArray.emplace_back(leafNode);
		}
		/// <summary>
		/// BVHを構築。
		/// </summary>
		/// <remark></remark>
		void Build();
	private:
		/// <summary>
		/// ノード
		/// </summary>
		struct SNode {
			AABB aabb;	// このノードのAABB
			SNode* rightNode = nullptr;	// 右のノード。
			SNode* leftNode = nullptr;	// 左のノード。
		};
		using SNodePtr = std::shared_ptr<SNode>;
		SNodePtr m_rootNode = nullptr;		// ルートノード。
		std::vector<SNodePtr> m_leafNodeArray;	// リーフノードの配列。

	};
}