#include "k2EnginePreCompile.h"
#include "CameraCollisionSolver.h"

namespace nsK2Engine {
	struct SConvexSweepCallback : public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		Vector3 m_rayDir;
		SConvexSweepCallback(Vector3 rayDir) :
			btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)),
			m_rayDir(rayDir) {}
		virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			Vector3 normal = Vector3(convexResult.m_hitNormalLocal.x(), convexResult.m_hitNormalLocal.y(), convexResult.m_hitNormalLocal.z());
			//normal.Set(convexResult.m_hitNormalLocal);

			if (convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Character
				|| convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
				return 1.0f;
			}
			return btCollisionWorld::ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}
	};
	CameraCollisionSolver::CameraCollisionSolver()
	{
	}
	CameraCollisionSolver::~CameraCollisionSolver()
	{
	}
	void CameraCollisionSolver::Init(float radius)
	{
		m_radius = radius;
		m_collider.Create(radius);
		m_isInited = true;	//初期化済みのフラグを立てる。
	}
	bool CameraCollisionSolver::Execute(Vector3& result, const Vector3& position, const Vector3& target)
	{
		if (m_isInited == false) {
			//TK_WARNING_MESSAGE("初期化されていないカメラコリジョンソルバーのExecute関数が呼ばれています。\n"
				//"CCameraCollisionSolver::Init関数を呼び出してください。\n");
			return false;

		}
		result = position;
		Vector3 vWk;
		vWk.Subtract(target, position);
		if (vWk.LengthSq() < FLT_EPSILON) {
			//視点と注視点がほぼ同じ座標にある。
			return false;
		}
		vWk.Normalize();
		//レイを作成する。
		btTransform btStart, btEnd;
		btStart.setIdentity();
		btEnd.setIdentity();
		btStart.setOrigin(btVector3(target.x, target.y, target.z));
		btEnd.setOrigin(btVector3(position.x, position.y, position.z));
		SConvexSweepCallback callback(vWk);
		//	callback.m_collisionFilterGroup = 
		PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collider.GetBody(), btStart, btEnd, callback);
		if (callback.hasHit()) {
			Vector3 vHitPos = Vector3(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
			Vector3 vOffset = Vector3(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
			vOffset.Scale(m_radius);
			result.Add(vHitPos, vOffset);
			return true;
		}
		return false;
	}
}