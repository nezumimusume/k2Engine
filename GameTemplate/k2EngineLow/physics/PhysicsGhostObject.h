#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// ゴーストオブジェクト。
	/// ゴーストオブジェクトは剛体がすり抜けるあたり判定のオブジェクトです。
	/// キャラクターのチェックポイント通過判定、クリア判定などの
	/// キャラクターがすり抜ける必要のあるあたり判定に使えます。
	/// </summary>
	class PhysicsGhostObject : public Noncopyable {
	public:
		~PhysicsGhostObject()
		{
			Release();
		}
		/// <summary>
		/// ゴーストオブジェクトを開放。
		/// 明示的なタイミングでゴーストオブジェクトを削除したい場合に呼び出してください。
		/// </summary>
		void Release()
		{
			if (m_isRegistPhysicsWorld == true) {
				PhysicsWorld::GetInstance()->RemoveCollisionObject(m_ghostObject);
				m_isRegistPhysicsWorld = false;
			}
		}
		/// <summary>
		/// ボックス形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos">座標。</param>
		/// <param name="rot">回転。</param>
		/// <param name="size">サイズ。</param>
		void CreateBox(Vector3 pos, Quaternion rot, Vector3 size);
		/// <summary>
		/// カプセル形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos">座標。</param>
		/// <param name="rot">回転。</param>
		/// <param name="radius">カプセルの半径。</param>
		/// <param name="height">カプセルの高さ。</param>
		void CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height);
		/// <summary>
		/// 球形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos">座標。</param>
		/// <param name="rot">回転。</param>
		/// <param name="radius">球形の半径。</param>
		void CreateSphere(Vector3 pos, Quaternion rot, float radius);
		/// <summary>
		/// メッシュ形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos">座標。</param>
		/// <param name="rot">回転。</param>
		/// <param name="model">モデル。</param>
		/// <param name="worldMatrix">ワールド行列。</param>
		void CreateMesh(Vector3 pos, Quaternion rot, const Model& model, const Matrix& worldMatrix);
		/// <summary>
		/// 引数で渡されたゴーストオブジェクトが自分自身かどうかを判定 
		/// </summary>
		/// <param name="ghost">ゴーストオブジェクト。</param>
		/// <returns>引数のゴーストオブジェクトが自分自身ならtrue。</returns>
		bool IsSelf(const btCollisionObject& ghost) const
		{
			return &ghost == &m_ghostObject;
		}
		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos">座標。</param>
		void SetPosition(const Vector3& pos)
		{
			auto& btTrans = m_ghostObject.getWorldTransform();
			btVector3 btPos;
			btPos = btVector3(pos.x, pos.y, pos.z);
			btTrans.setOrigin(btPos);
		}
		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <param name="rot">回転。</param>
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
		/// ゴースト作成処理の共通処理。
		/// </summary>
		/// <param name="pos">座標。</param>
		/// <param name="rot">回転。</param>
		void CreateCommon(Vector3 pos, Quaternion rot);
	private:
		bool							m_isRegistPhysicsWorld = false;	//物理ワールドに登録しているかどうかのフラグ。
		btGhostObject					m_ghostObject;					//ゴースト
		std::unique_ptr<ICollider>		m_collider;						//コライダー。
	};
}
