
#pragma once

#include "geometry/AABB.h"

namespace nsK2EngineLow {

	/// <summary>
	/// BSP
	/// </summary>
	
	class BSP {
	private:
		/// <summary>
		/// ���ʁB
		/// </summary>
		struct SPlane {
			Vector3 normal;		// ���ʂ̖@��
			float distance;		// ���ʂ܂ł̋����B
		};
	public:
		enum EnCompositeType {
			enCompositeType_Node,
			enCompositeType_Leaf,
			enCompositeType_LeafList
		};
		struct IComposite {
			int type = enCompositeType_Node;
		};

		using ICompositePtr = std::shared_ptr<IComposite>;
		/// <summary>
		/// �m�[�h
		/// </summary>
		struct SNode : public IComposite {
			SPlane plane;		// �������ʁB
			ICompositePtr rightNode;	// �E�̃m�[�h�B
			ICompositePtr leftNode;	// ���̃m�[�h�B
		};
		/// <summary>
		/// ���[�t
		/// </summary>
		struct SLeaf : public IComposite {
			Vector3 position;
			void* extraData;
		};
		/// <summary>
		/// ���[�t�̃��X�g
		/// </summary>
		struct SLeafList : public IComposite {
			std::vector< ICompositePtr> leafList;
		};
	private:
		
		ICompositePtr m_rootNode = nullptr;			// ���[�g�m�[�h�B
		std::vector<ICompositePtr> m_leafNodeArray;	// ���[�t�m�[�h�̔z��B
	public:
		/// <summary>
		/// ���[�t��ǉ��B
		/// </summary>
		/// <param name="aabb"></param>
		void AddLeaf(const Vector3& position, void* extraData) 
		{
			// ���[�t�m�[�h�����B
			ICompositePtr leafNode = std::make_shared<SLeaf>();
			m_leafNodeArray.emplace_back(leafNode);
			SLeaf* leaf = static_cast<SLeaf*>(leafNode.get());
			leaf->position = position;
			leaf->extraData = extraData;
			leaf->type = enCompositeType_Leaf;
			
		}
		/// <summary>
		/// BVH���\�z�B
		/// </summary>
		/// <remark></remark>
		void Build();
		/// <summary>
		/// BSP�c���[��T������
		/// </summary>
		/// <remark>
		/// BSP�c���[��T�����āA�}�ɓ��B����ƁA�����Ŏw�肳�ꂽ�R�[���o�b�N�֐����Ă΂�܂��B
		/// </remark>
		/// <param name="pos">���W</param>
		/// <param name="onEndWalk">�T�����I���������ɌĂ΂�鏈��</param>
		void WalkTree(const Vector3& pos, std::function<void(IComposite* leaf)> onEndWalk);
	private:
		/// <summary>
		/// ���ʂŃ��[�t�m�[�h�𕪊����Ă���
		/// </summary>
		void SplitLeafArray(
			std::vector<ICompositePtr>& leftLeafNodeArray,
			std::vector<ICompositePtr>& rightLeafNodeArray,
			const SPlane& plane, 
			const std::vector<ICompositePtr>& leafNodeArray
		);
		/// <summary>
		/// �����U�s�񂩂番�����ʂ��v�Z����B
		/// </summary>
		/// <param name="plane"></param>
		/// <param name="covarianceMatrix"></param>
		void CalcSplitPlaneFromCovarianceMatrix(
			SPlane& plane,
			float covarianceMatrix[3][3],
			const Vector3& centerPos
		);
		/// <summary>
		/// ���[�t�m�[�h�̔z�񂩂狤���U�s����v�Z����B
		/// </summary>
		/// <param name="covarianceMatrix">�����U�s��̌v�Z��</param>
		/// <param name="leafNodeArray">���[�t�m�[�h�̔z��</param>
		/// <param name="centerPos">���[�t�m�[�h�̒��S���W</param>
		void CalcCovarianceMatrixFromLeafNodeList(
			float covarianceMatrix[3][3], 
			const std::vector<ICompositePtr>& leafNodeArray, 
			const Vector3& centerPos
		);
		/// <summary>
		/// ���[�t�̃��X�g���璆�S���W���v�Z����B
		/// </summary>
		/// <param name="leafNodeArray"></param>
		/// <returns></returns>
		Vector3 CalcCenterPositionFromLeafList(
			const std::vector<ICompositePtr>& leafNodeArray
		);
		/// <summary>
		/// BVH���\�z�B
		/// </summary>
		/// <param name="leafNodeArray"></param>
		ICompositePtr BuildInternal(const std::vector<ICompositePtr>& leafNodeArray);
		
	
		void WalkTree(ICompositePtr node, const Vector3& pos, std::function<void(IComposite* leaf)> onEndWalk );
	};
}