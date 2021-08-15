#pragma once

#include "ICollider.h"

namespace nsK2EngineLow {
	/// <summary>
	/// ���b�V���R���C�_�[
	/// </summary>
	class MeshCollider : public ICollider
	{
	public:
		/// <summary>
		/// Model�N���X���烁�b�V���R���C�_�[���쐬�B
		/// </summary>
		/// <param name="model"></param>
		/// <param name="worldMatrix"></param>
		void CreateFromModel(const Model& model, const Matrix& worldMatrix);
		/// <summary>
		/// BulletPhysics�̃R���C�_�[�̌`����擾�B
		/// </summary>
		/// <returns></returns>
		btCollisionShape* GetBody() const override
		{
			return m_meshShape.get();
		}
	private:
		typedef std::vector<Vector3>		VertexBuffer;		//���_�o�b�t�@�B
		typedef std::vector<unsigned int>	IndexBuffer;		//�C���f�b�N�X�o�b�t�@�B
		typedef std::unique_ptr<VertexBuffer>	VertexBufferPtr;
		typedef std::unique_ptr<IndexBuffer>	IndexBufferPtr;
		std::vector<VertexBufferPtr>					m_vertexBufferArray;		//���_�o�b�t�@�̔z��B
		std::vector<IndexBufferPtr>						m_indexBufferArray;		//�C���f�b�N�X�o�b�t�@�̔z��B
		std::unique_ptr<btBvhTriangleMeshShape>			m_meshShape;				//���b�V���`��B
		std::unique_ptr<btTriangleIndexVertexArray>		m_stridingMeshInterface;
	};
}
