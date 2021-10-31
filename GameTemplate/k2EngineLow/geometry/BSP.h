
#pragma once

#include "geometry/AABB.h"

namespace nsK2EngineLow {

	/// <summary>
	/// BSP�c���[
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
		/// <summary>
		/// BSP�c���[�̗v�f�̎��
		/// </summary>
		enum EnEntityType {
			enEntityType_Node,
			enEntityType_Leaf,
			enEntityType_LeafList
		};
		/// <summary>
		/// BSP�c���[�̗v�f
		/// </summary>
		struct SEntity {
			int type = enEntityType_Node;
		};

		using SEntityPtr = std::shared_ptr<SEntity>;
		/// <summary>
		/// �m�[�h
		/// </summary>
		struct SNode : public SEntity  {
			SPlane plane;				// �������ʁB
			SEntityPtr rightEntity;		// �E�̗v�f
			SEntityPtr leftEntity;		// ���̗v�f�B
			Vector3 centerPos;			// ���S���W�B
			std::vector<SEntityPtr> leafArray;	// �m�[�h������Ă��郊�[�t�B
		};
		/// <summary>
		/// ���[�t
		/// </summary>
		struct SLeaf : public SEntity {
			Vector3 position;
			void* extraData;
		};
		/// <summary>
		/// ���[�t�̃��X�g
		/// </summary>
		struct SLeafList : public SEntity {
			std::vector< SEntityPtr > leafList;
		};
	private:
		
		SEntityPtr m_rootNode = nullptr;		// ���[�g�m�[�h�B
		std::vector<SEntityPtr> m_leafArray;	// ���[�t�̔z��B
	public:
		/// <summary>
		/// ���[�t��ǉ��B
		/// </summary>
		/// <param name="aabb"></param>
		void AddLeaf(const Vector3& position, void* extraData) 
		{
			// ���[�t�����B
			auto newEntity = std::make_shared<SLeaf>();
			m_leafArray.emplace_back(newEntity);
			SLeaf* leaf = static_cast<SLeaf*>(newEntity.get());
			leaf->position = position;
			leaf->extraData = extraData;
			leaf->type = enEntityType_Leaf;
			
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
		/// BSP�c���[��T�����āA���[�t�ɓ��B����ƁA�����Ŏw�肳�ꂽ�R�[���o�b�N�֐����Ă΂�܂��B
		/// </remark>
		/// <param name="pos">���W</param>
		/// <param name="onEndWalk">�T�����I���������ɌĂ΂�鏈��</param>
		void WalkTree(const Vector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const;
	private:
		/// <summary>
		/// ���ʂŃ��[�t�m�[�h�𕪊����Ă���
		/// </summary>
		void SplitLeafArray(
			std::vector<SEntityPtr>& leftLeafArray,
			std::vector<SEntityPtr>& rightLeafArray,
			const SPlane& plane, 
			const std::vector<SEntityPtr>& leafArray
		);
		/// <summary>
		/// �����U�s�񂩂番�����ʂ��v�Z����B
		/// </summary>
		/// <param name="plane"></param>
		/// <param name="covarianceMatrix"></param>
		void CalcSplitPlaneFromCovarianceMatrix(
			SPlane& plane,
			float covarianceMatrix[3][3],
			const Vector3& centerPos,
			const std::vector<SEntityPtr>& leafArray
		);
		/// <summary>
		/// ���[�t�m�[�h�̔z�񂩂狤���U�s����v�Z����B
		/// </summary>
		/// <param name="covarianceMatrix">�����U�s��̌v�Z��</param>
		/// <param name="leafNodeArray">���[�t�m�[�h�̔z��</param>
		/// <param name="centerPos">���[�t�m�[�h�̒��S���W</param>
		void CalcCovarianceMatrixFromLeafNodeList(
			float covarianceMatrix[3][3], 
			const std::vector<SEntityPtr>& leafArray,
			const Vector3& centerPos
		);
		/// <summary>
		/// ���[�t�̃��X�g���璆�S���W���v�Z����B
		/// </summary>
		/// <param name="leafNodeArray"></param>
		/// <returns></returns>
		Vector3 CalcCenterPositionFromLeafList(
			const std::vector<SEntityPtr>& leafArray
		);
		/// <summary>
		/// �V����BSP�c���[�̗v�f���쐬����B
		/// </summary>
		/// <param name="leafNodeArray"></param>
		SEntityPtr CreateBSPTreeEntity(const std::vector<SEntityPtr>& leafArray);
		/// <summary>
		/// BSP�c���[��LeafList�v�f���쐬����B
		/// </summary>
		/// <param name="leafArray"></param>
		/// <returns></returns>
		SEntityPtr CreateBSPTreeEntity_LeafList(const std::vector<SEntityPtr>& leafArray);
	
		void WalkTree(SEntityPtr entity, const Vector3& pos, std::function<void(SLeaf* leaf)> onEndWalk ) const;
	};
}