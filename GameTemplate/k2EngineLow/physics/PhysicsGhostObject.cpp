#include "k2EngineLowPreCompile.h"
#include "PhysicsGhostObject.h"

namespace nsK2EngineLow {
	void PhysicsGhostObject::CreateCommon(Vector3 pos, Quaternion rot)
	{
		m_ghostObject.setCollisionShape(m_collider->GetBody());
		btTransform btTrans;
		btTrans.setOrigin({ pos.x, pos.y, pos.z });
		btTrans.setRotation({ rot.x, rot.y, rot.z, rot.w });
		m_ghostObject.setWorldTransform(btTrans);

		//•¨—ƒGƒ“ƒWƒ“‚É“o˜^B
		PhysicsWorld::GetInstance()->AddCollisionObject(m_ghostObject);
		m_isRegistPhysicsWorld = true;
	}

	void PhysicsGhostObject::CreateBox(Vector3 pos, Quaternion rot, Vector3 size)
	{
		Release();
		auto boxCollider = std::make_unique<BoxCollider>();
		boxCollider->Create(size);
		m_collider = std::move(boxCollider);
		CreateCommon(pos, rot);
	}

	void PhysicsGhostObject::CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height)
	{
		Release();
		auto capusuleCollider = std::make_unique<CCapsuleCollider>();
		capusuleCollider->Init(radius, height);
		m_collider = std::move(capusuleCollider);
		CreateCommon(pos, rot);
	}

	void PhysicsGhostObject::CreateSphere(Vector3 pos, Quaternion rot, float radius)
	{
		Release();
		auto sphereCollider = std::make_unique<SphereCollider>();
		sphereCollider->Create(radius);
		m_collider = std::move(sphereCollider);
		CreateCommon(pos, rot);
	}

	void PhysicsGhostObject::CreateMesh(Vector3 pos, Quaternion rot, const Model& model, const Matrix& worldMatrix)
	{
		Release();
		auto sphereCollider = std::make_unique<MeshCollider>();
		sphereCollider->CreateFromModel(model, worldMatrix);
		m_collider = std::move(sphereCollider);
		CreateCommon(pos, rot);
	}
}