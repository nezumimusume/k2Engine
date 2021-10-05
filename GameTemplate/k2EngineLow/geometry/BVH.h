
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
		/// AABB��ǉ�
		/// </summary>
		/// <param name="aabb"></param>
		void AddLeaf(const AABB& aabb) 
		{
			// ���[�t�m�[�h�����B
			auto leafNode = std::make_shared<SNode>();
			leafNode->aabb = aabb;
			m_leafNodeArray.emplace_back(leafNode);
		}
		/// <summary>
		/// BVH���\�z�B
		/// </summary>
		/// <remark></remark>
		void Build();
	private:
		/// <summary>
		/// �m�[�h
		/// </summary>
		struct SNode {
			AABB aabb;	// ���̃m�[�h��AABB
			SNode* rightNode = nullptr;	// �E�̃m�[�h�B
			SNode* leftNode = nullptr;	// ���̃m�[�h�B
		};
		using SNodePtr = std::shared_ptr<SNode>;
		SNodePtr m_rootNode = nullptr;		// ���[�g�m�[�h�B
		std::vector<SNodePtr> m_leafNodeArray;	// ���[�t�m�[�h�̔z��B

	};
}