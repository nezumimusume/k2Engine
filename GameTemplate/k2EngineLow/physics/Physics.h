#pragma once

#include <memory>
#include "DebugWireframe.h"

namespace nsK2EngineLow {
	class CharacterController;

	class PhysicsWorld : public Noncopyable
	{
		static PhysicsWorld* m_instance;	//唯一のインスタンス。
		std::unique_ptr<btDefaultCollisionConfiguration> 	 m_collisionConfig;
		std::unique_ptr<btCollisionDispatcher>				 m_collisionDispatcher;	//!<衝突解決処理。
		std::unique_ptr<btBroadphaseInterface>				 m_overlappingPairCache;	//!<ブロードフェーズ。衝突判定の枝切り。
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraintSolver;		//!<コンストレイントソルバー。拘束条件の解決処理。
		std::unique_ptr<btDiscreteDynamicsWorld>			 m_dynamicWorld;			//!<ワールド。
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
				//実際にdrawLineを呼んでます。
				m_dynamicWorld->debugDrawWorld();
				m_debugWireFrame.End(rc);
			}
#endif
		}

		//当たり判定描画を有効にする。
		void EnableDrawDebugWireFrame()
		{
#ifdef _DEBUG
			m_isDrawDebugWireFrame = true;
#endif
		}

		//当たり判定描画を無効にする。
		void DisableDrawDebugWireFrame()
		{
#ifdef _DEBUG
			m_isDrawDebugWireFrame = false;
#endif
		}

		void Release();
		/*!
		* @brief	重力を設定。。
		*/
		void SetGravity(Vector3 g)
		{
			m_dynamicWorld->setGravity(btVector3(g.x, g.y, g.z));
		}
		/*!
		* @brief	ダイナミックワールドを取得。
		*/
		btDiscreteDynamicsWorld* GetDynamicWorld()
		{
			return m_dynamicWorld.get();
		}

		/*!
		* @brief	剛体を登録。
		*/
		void AddRigidBody(RigidBody& rb)
		{
			m_dynamicWorld->addRigidBody(rb.GetBody());
		}


		/*!
		* @brief	剛体を破棄。
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
		/// 物理ワールドに対して、凸型コライダーSweepテストを行う。
		/// </summary>
		/// <param name="collider">コライダー</param>
		/// <param name="convexStart">コライダーの開始座標</param>
		/// <param name="rayEnd">コライダーの終了座標</param>
		/// <returns>trueがかえってきたら当たっている。</returns>
		bool ConvexSweepTest(ICollider& collider, const Vector3& rayStart, const Vector3& rayEnd) const;
		/// <summary>
		/// レイテストを実施。
		/// </summary>
		/// <param name="rayStart">レイの始点</param>
		/// <param name="rayEnd">レイの終点</param>
		/// <param name="hitPos">交点の格納先</param>
		/// <returns>trueが返ってきたら衝突している。</returns>
		bool RayTest(const Vector3& rayStart, const Vector3& rayEnd, Vector3& hitPos) const;
		/*!
		* @brief	コリジョンオブジェクトをワールドに登録。
		*@param[in]	colliObj	コリジョンオブジェクト。
		*/
		void AddCollisionObject(btCollisionObject& colliObj)
		{
			m_dynamicWorld->addCollisionObject(&colliObj);
		}
		/*!
		* @brief	コリジョンオブジェクトをワールドから削除。
		*@param[in]	colliObj	コリジョンオブジェクト。
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
