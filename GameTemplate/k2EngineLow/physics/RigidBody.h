#pragma once

namespace nsK2EngineLow {
	class ICollider;
	//���̏��������B
	struct RigidBodyInitData {
		Vector3 pos;						//���W�B
		Quaternion rot;					//��]�B
		ICollider* collider = nullptr;	//�R���C�_�[�B
		float mass = 0;					//���ʁB
		float restitution = 0.0f;			//�����́A(���l���グ��΁A���̂����˕Ԃ�悤�ɂȂ�)�B
	};

	/// <summary>
	/// ���̃N���X�B
	/// </summary>
	class RigidBody : public Noncopyable {
	public:
		~RigidBody();
		/// <summary>
		/// ���̂��������B
		/// </summary>
		/// <param name="rbInfo">�������f�[�^�̏��</param>
		void Init(RigidBodyInitData& initData);
		/// <summary>
		/// bulletPhysics�̍��̂��擾�B
		/// </summary>
		/// <returns></returns>
		btRigidBody* GetBody()
		{
			return m_rigidBody.get();
		}
		/*!
		* @brief	�����I�u�W�F�N�g�̍��W�Ɖ�]���擾
		*@param[out]	pos			���W�B
		*@param[out]	rot			��]�B
		*/
		void GetPositionAndRotation(Vector3& pos, Quaternion& rot) const
		{
			btTransform trans;
			m_myMotionState->getWorldTransform(trans);
			auto btPos = trans.getOrigin();
			pos.Set(btPos.x(), btPos.y(), btPos.z());
			auto btRot = trans.getRotation();
			rot.Set(btRot.x(), btRot.y(), btRot.z(), btRot.w());
		}
		/*!
		* @brief	�����I�u�W�F�N�g�̍��W�Ɖ�]��ݒ�
		*@param[in]	pos			���W�B
		*@param[in]	rot			��]�B
		*/
		void SetPositionAndRotation(const Vector3& pos, const Quaternion& rot)
		{
			btTransform trans;
			btVector3 btPos;
			btPos.setValue(pos.x, pos.y, pos.z);
			trans.setOrigin(btPos);
			btQuaternion btRot;
			btRot.setValue(rot.x, rot.y, rot.z, rot.w);
			trans.setRotation(btRot);
			m_rigidBody->setWorldTransform(trans);
		}


		/*!
		* @brief	�͂�������B
		*@param[out]	force		�́B
		*@param[out]	relPos		�͂���������W�B
		*/
		void AddForce(const Vector3& force, const Vector3& relPos)
		{
			btVector3 btForce;
			btVector3 btRelPos;
			btForce.setValue(force.x, force.y, force.z);
			btRelPos.setValue(relPos.x, relPos.y, relPos.z);
			m_rigidBody->applyForce(btForce, btRelPos);
			m_rigidBody->activate();
		}
		/// <summary>
		/// ���x��ݒ�B
		/// </summary>
		/// <param name="vel"></param>
		void SetLinearVelocity(const Vector3& vel)
		{
			btVector3 btVel;
			btVel.setValue(vel.x, vel.y, vel.z);
			m_rigidBody->setLinearVelocity(btVel);
			m_rigidBody->activate();
		}
		/// <summary>
		/// ���݂̑��x���擾�B
		/// </summary>
		/// <returns></returns>
		Vector3 GetLinearVelocity() const
		{
			auto& btVel = m_rigidBody->getLinearVelocity();
			Vector3 vel;
			vel.Set(btVel.x(), btVel.y(), btVel.z());
			return vel;
		}
		/// <summary>
		/// ���C�͂�ݒ肷��B
		/// </summary>
		/// <param name="friction"></param>
		void SetFriction(float friction)
		{
			m_rigidBody->setFriction(friction);
			m_rigidBody->setRollingFriction(friction);
		}
		/// <summary>
		/// �ړ��\�Ȏ���ݒ�B
		/// </summary>
		/// <param name="linearFactor"></param>
		void SetLinearFactor(float x, float y, float z)
		{
			Vector3 v = { x, y, z };
			SetLinearFactor(v);
		}
		void SetLinearFactor(Vector3& linearFactor)
		{
			btVector3 v;
			v.setValue(linearFactor.x, linearFactor.y, linearFactor.z);
			m_rigidBody->setLinearFactor(v);
		}
		/// <summary>
		/// �p���x��ݒ肷��
		/// </summary>
		/// <param name="vel"></param>
		void SetAngularVelocity(Vector3 vel)
		{
			btVector3 v;
			v.setValue(vel.x, vel.y, vel.z);
			m_rigidBody->setAngularVelocity(v);
			m_rigidBody->activate();
		}
		/// <summary>
		/// ��]�\�Ȏ���ݒ肷��B
		/// </summary>
		/// <param name="angluarFactor"></param>
		void SetAngularFactor(Vector3 angluarFactor)
		{
			btVector3 v;
			v.setValue(angluarFactor.x, angluarFactor.y, angluarFactor.z);
			m_rigidBody->setAngularFactor(v);
		}
		void SetAngularFactor(float x, float y, float z)
		{
			SetAngularFactor({ x, y, z });
		}
		void Release();
	private:
		std::unique_ptr<btRigidBody>			m_rigidBody;		//���́B
		std::unique_ptr<btDefaultMotionState>	m_myMotionState;	//���[�V�����X�e�[�g�B
	};
}