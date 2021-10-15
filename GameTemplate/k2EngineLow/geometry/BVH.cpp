#include "k2EngineLowPreCompile.h"
#include "geometry/BVH.h"

namespace nsK2EngineLow {
	/// <summary>
	/// BVH���\�z�B
	/// </summary>
	/// <remark></remark>
	void BVH::Build()
	{
		// ���[�g�m�[�h���쐬�B
		m_rootNode = std::make_shared<SNode>();
		// �m�[�h������AABB���v�Z����B
		Vector3 vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		Vector3 vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
		for (auto& node : m_leafNodeArray) {
			vMax.Max(node->aabb.GetMax());
			vMin.Min(node->aabb.GetMin());
		}
		// �m�[�h��AABB������������B
		m_rootNode->aabb.Init(vMax, vMin);
		
		// �听�����͂��s���āA�������ʂ����߂�B
		// �܂��́AAABB�̒��S���W�����߂�B
		Vector3 centerPos;
		for (auto& node : m_leafNodeArray) {
			centerPos += node->aabb.GetCenter();
		}
		centerPos /= m_leafNodeArray.size();
		// �����ċ����U�s����v�Z����
		Matrix covarianceMatrix;
		for (auto& node : m_leafNodeArray) {
			covarianceMatrix[0][0] += node->aabb.GetCenter();
		}
	}
}