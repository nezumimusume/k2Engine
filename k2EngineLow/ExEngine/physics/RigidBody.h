#pragma once

class ICollider;
//剛体初期化情報。
struct RigidBodyInitData {
	Vector3 pos;					//座標。
	Quaternion rot;					//回転。
	Vector3 localInteria;			//慣性テンソル。回転のしやすさ的なもの。
	ICollider* collider = nullptr;	//コライダー。
	float mass = 0;					//質量。

};

/// <summary>
/// 剛体クラス。
/// </summary>
class RigidBody {
public:
	~RigidBody();
	/// <summary>
	/// 剛体を初期化。
	/// </summary>
	/// <param name="rbInfo">初期化データの情報</param>
	void Init(RigidBodyInitData& initData);
	/// <summary>
	/// bulletPhysicsの剛体を取得。
	/// </summary>
	/// <returns></returns>
	btRigidBody* GetBody()
	{
		return m_rigidBody.get();
	}
	/*!
	* @brief	物理オブジェクトの座標と回転を取得
	*@param[out]	pos			座標。
	*@param[out]	rot			回転。
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
	* @brief	物理オブジェクトの座標と回転を設定
	*@param[in]	pos			座標。
	*@param[in]	rot			回転。
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
	* @brief	力を加える。
	*@param[out]	force		力。
	*@param[out]	relPos		力を加える座標。
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
	/// 速度を設定。
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
	/// 現在の速度を取得。
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
	/// 摩擦力を設定する。
	/// </summary>
	/// <param name="friction"></param>
	void SetFriction(float friction)
	{
		m_rigidBody->setFriction(friction);
		m_rigidBody->setRollingFriction(friction);
	}
	/// <summary>
	/// 移動可能な軸を設定。
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
	/// 角速度を設定する
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
	/// 回転可能な軸を設定する。
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
private:
	std::unique_ptr<btRigidBody>			m_rigidBody;		//剛体。
	std::unique_ptr<btDefaultMotionState>	m_myMotionState;	//モーションステート。
};
