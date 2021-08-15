#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �S�[�X�g�I�u�W�F�N�g�B
	/// �S�[�X�g�I�u�W�F�N�g�͍��̂����蔲���邠���蔻��̃I�u�W�F�N�g�ł��B
	/// �L�����N�^�[�̃`�F�b�N�|�C���g�ʉߔ���A�N���A����Ȃǂ�
	/// �L�����N�^�[�����蔲����K�v�̂��邠���蔻��Ɏg���܂��B
	/// </summary>
	class PhysicsGhostObject : public Noncopyable {
	public:
		~PhysicsGhostObject()
		{
			Release();
		}
		/// <summary>
		/// �S�[�X�g�I�u�W�F�N�g���J���B
		/// �����I�ȃ^�C�~���O�ŃS�[�X�g�I�u�W�F�N�g���폜�������ꍇ�ɌĂяo���Ă��������B
		/// </summary>
		void Release()
		{
			if (m_isRegistPhysicsWorld == true) {
				PhysicsWorld::GetInstance()->RemoveCollisionObject(m_ghostObject);
				m_isRegistPhysicsWorld = false;
			}
		}
		/// <summary>
		/// �{�b�N�X�`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="size">�T�C�Y�B</param>
		void CreateBox(Vector3 pos, Quaternion rot, Vector3 size);
		/// <summary>
		/// �J�v�Z���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="radius">�J�v�Z���̔��a�B</param>
		/// <param name="height">�J�v�Z���̍����B</param>
		void CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height);
		/// <summary>
		/// ���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="radius">���`�̔��a�B</param>
		void CreateSphere(Vector3 pos, Quaternion rot, float radius);
		/// <summary>
		/// ���b�V���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		/// <param name="model">���f���B</param>
		/// <param name="worldMatrix">���[���h�s��B</param>
		void CreateMesh(Vector3 pos, Quaternion rot, const Model& model, const Matrix& worldMatrix);
		/// <summary>
		/// �����œn���ꂽ�S�[�X�g�I�u�W�F�N�g���������g���ǂ����𔻒� 
		/// </summary>
		/// <param name="ghost">�S�[�X�g�I�u�W�F�N�g�B</param>
		/// <returns>�����̃S�[�X�g�I�u�W�F�N�g���������g�Ȃ�true�B</returns>
		bool IsSelf(const btCollisionObject& ghost) const
		{
			return &ghost == &m_ghostObject;
		}
		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="pos">���W�B</param>
		void SetPosition(const Vector3& pos)
		{
			auto& btTrans = m_ghostObject.getWorldTransform();
			btVector3 btPos;
			btPos = btVector3(pos.x, pos.y, pos.z);
			btTrans.setOrigin(btPos);
		}
		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rot">��]�B</param>
		void SetRotation(const Quaternion& rot)
		{
			auto& btTrans = m_ghostObject.getWorldTransform();
			btQuaternion btRot;
			btRot = btQuaternion(rot.x, rot.y, rot.z, rot.w);
			btTrans.setRotation(btRot);
		}
		btCollisionObject& GetbtCollisionObject()
		{
			return m_ghostObject;
		}
	private:
		/// <summary>
		/// �S�[�X�g�쐬�����̋��ʏ����B
		/// </summary>
		/// <param name="pos">���W�B</param>
		/// <param name="rot">��]�B</param>
		void CreateCommon(Vector3 pos, Quaternion rot);
	private:
		bool							m_isRegistPhysicsWorld = false;	//�������[���h�ɓo�^���Ă��邩�ǂ����̃t���O�B
		btGhostObject					m_ghostObject;					//�S�[�X�g
		std::unique_ptr<ICollider>		m_collider;						//�R���C�_�[�B
	};
}
