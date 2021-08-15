#include "k2EngineLowPreCompile.h"
#include "RigidBody.h"

namespace nsK2EngineLow {
	void RigidBody::Init(RigidBodyInitData& initData)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(initData.pos.x, initData.pos.y, initData.pos.z));
		transform.setRotation(btQuaternion(initData.rot.x, initData.rot.y, initData.rot.z, initData.rot.w));

		m_myMotionState = std::make_unique<btDefaultMotionState>();
		m_myMotionState->setWorldTransform(transform);
		btVector3 btLocalInteria;
		btLocalInteria.setX(initData.localInteria.x);
		btLocalInteria.setY(initData.localInteria.y);
		btLocalInteria.setZ(initData.localInteria.z);

		btRigidBody::btRigidBodyConstructionInfo btRbInfo(initData.mass, m_myMotionState.get(), initData.collider->GetBody(), btLocalInteria);
		//îΩî≠óÕÇê›íËÅB
		btRbInfo.m_restitution = initData.restitution;
		m_rigidBody = std::make_unique<btRigidBody>(btRbInfo);

		PhysicsWorld::GetInstance()->AddRigidBody(*this);
	}
	RigidBody::~RigidBody()
	{
		Release();
	}

	void RigidBody::Release()
	{
		if (m_rigidBody) {
			PhysicsWorld::GetInstance()->RemoveRigidBody(*this);
		}
	}
}