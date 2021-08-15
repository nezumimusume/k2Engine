/*!
* @brief	�ÓI�I�u�W�F�N�g�B
*/


#include "k2EngineLowPreCompile.h"
#include "physics/PhysicsStaticObject.h"

namespace nsK2EngineLow {
	PhysicsStaticObject::PhysicsStaticObject()
	{
	}
	PhysicsStaticObject::~PhysicsStaticObject()
	{
	}
	void PhysicsStaticObject::CreateFromModel(Model& model, const Matrix& worldMatrix, const float restitution)
	{
		m_meshCollider.CreateFromModel(model, worldMatrix);
		RigidBodyInitData rbInfo;
		rbInfo.collider = &m_meshCollider;
		rbInfo.mass = 0.0f;
		rbInfo.restitution = restitution;
		m_rigidBody.Init(rbInfo);
	}
}
