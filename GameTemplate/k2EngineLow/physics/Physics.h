#pragma once

#include <memory>
#include "DebugWireframe.h"

namespace nsK2EngineLow {
	class CharacterController;

	class PhysicsWorld : public Noncopyable
	{
		static PhysicsWorld* m_instance;	//�B��̃C���X�^���X�B
		std::unique_ptr<btDefaultCollisionConfiguration> 	 m_collisionConfig;
		std::unique_ptr<btCollisionDispatcher>				 m_collisionDispatcher;	//!<�Փˉ��������B
		std::unique_ptr<btBroadphaseInterface>				 m_overlappingPairCache;	//!<�u���[�h�t�F�[�Y�B�Փ˔���̎}�؂�B
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraintSolver;		//!<�R���X�g���C���g�\���o�[�B�S�������̉��������B
		std::unique_ptr<btDiscreteDynamicsWorld>			 m_dynamicWorld;			//!<���[���h�B
#ifdef _DEBUG
		DebugWireframe m_debugWireFrame;
		bool m_isDrawDebugWireFrame = false;
#endif

	public:
		static void CreateInstance()
		{
			m_instance = new PhysicsWorld();
		}
		static PhysicsWorld* GetInstance()
		{
			return m_instance;
		}
		static void DeleteInstance()
		{
			delete m_instance;
		}

		void Update(float deltaTime);
		void DebubDrawWorld(RenderContext& rc)
		{
#ifdef _DEBUG
			if (m_isDrawDebugWireFrame) {
				m_debugWireFrame.Begin();
				//���ۂ�drawLine���Ă�ł܂��B
				m_dynamicWorld->debugDrawWorld();
				m_debugWireFrame.End(rc);
			}
#endif
		}

		//�����蔻��`���L���ɂ���B
		void EnableDrawDebugWireFrame()
		{
#ifdef _DEBUG
			m_isDrawDebugWireFrame = true;
#endif
		}

		//�����蔻��`��𖳌��ɂ���B
		void DisableDrawDebugWireFrame()
		{
#ifdef _DEBUG
			m_isDrawDebugWireFrame = false;
#endif
		}

		void Release();
		/*!
		* @brief	�d�͂�ݒ�B�B
		*/
		void SetGravity(Vector3 g)
		{
			m_dynamicWorld->setGravity(btVector3(g.x, g.y, g.z));
		}
		/*!
		* @brief	�_�C�i�~�b�N���[���h���擾�B
		*/
		btDiscreteDynamicsWorld* GetDynamicWorld()
		{
			return m_dynamicWorld.get();
		}

		/*!
		* @brief	���̂�o�^�B
		*/
		void AddRigidBody(RigidBody& rb)
		{
			m_dynamicWorld->addRigidBody(rb.GetBody());
		}


		/*!
		* @brief	���̂�j���B
		*/
		void RemoveRigidBody(RigidBody& rb)
		{
			m_dynamicWorld->removeRigidBody(rb.GetBody());
		}
		
		void ConvexSweepTest(
			const btConvexShape* castShape,
			const btTransform& convexFromWorld,
			const btTransform& convexToWorld,
			btCollisionWorld::ConvexResultCallback& resultCallback,
			btScalar allowedCcdPenetration = 0.0f
		) const
		{
			m_dynamicWorld->convexSweepTest(castShape, convexFromWorld, convexToWorld, resultCallback, allowedCcdPenetration);
		}
		/// <summary>
		/// �������[���h�ɑ΂��āA�ʌ^�R���C�_�[Sweep�e�X�g���s���B
		/// </summary>
		/// <param name="collider">�R���C�_�[</param>
		/// <param name="convexStart">�R���C�_�[�̊J�n���W</param>
		/// <param name="rayEnd">�R���C�_�[�̏I�����W</param>
		/// <returns>true���������Ă����瓖�����Ă���B</returns>
		bool ConvexSweepTest(ICollider& collider, const Vector3& rayStart, const Vector3& rayEnd) const;
		/// <summary>
		/// ���C�e�X�g�����{�B
		/// </summary>
		/// <param name="rayStart">���C�̎n�_</param>
		/// <param name="rayEnd">���C�̏I�_</param>
		/// <param name="hitPos">��_�̊i�[��</param>
		/// <returns>true���Ԃ��Ă�����Փ˂��Ă���B</returns>
		bool RayTest(const Vector3& rayStart, const Vector3& rayEnd, Vector3& hitPos) const;
		/*!
		* @brief	�R���W�����I�u�W�F�N�g�����[���h�ɓo�^�B
		*@param[in]	colliObj	�R���W�����I�u�W�F�N�g�B
		*/
		void AddCollisionObject(btCollisionObject& colliObj)
		{
			m_dynamicWorld->addCollisionObject(&colliObj);
		}
		/*!
		* @brief	�R���W�����I�u�W�F�N�g�����[���h����폜�B
		*@param[in]	colliObj	�R���W�����I�u�W�F�N�g�B
		*/
		void RemoveCollisionObject(btCollisionObject& colliObj)
		{
			m_dynamicWorld->removeCollisionObject(&colliObj);
		}

		void ContactTest(
			btCollisionObject* colObj,
			std::function<void(const btCollisionObject& contactCollisionObject)> cb
		);
		void ContactTest(
			RigidBody& rb,
			std::function<void(const btCollisionObject& contactCollisionObject)> cb
		);

		void ContactTest(
			CharacterController& charaCon,
			std::function<void(const btCollisionObject& contactCollisionObject)> cb
		);

	private:
		PhysicsWorld();
		~PhysicsWorld();
		void Init();
	};
}
