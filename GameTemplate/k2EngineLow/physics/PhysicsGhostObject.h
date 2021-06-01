#pragma once

/*!
	 * @brief	ゴーストオブジェクト
	 *@detail
	 * ゴーストオブジェクトは剛体がすり抜けるあたり判定のオブジェクトです。
	 * キャラクターのチェックポイント通過判定、クリア判定などの
	 * キャラクターがすり抜ける必要のあるあたり判定に使えます。
	 */
class PhysicsGhostObject{
public:
	~PhysicsGhostObject()
	{
		Release();
	}
	/*!
	* @brief	ゴーストオブジェクトを解放。
	*@detail
	* 明示的なタイミングでゴーストオブジェクトを削除したい場合に呼び出してください。
	*/
	void Release()
	{
		if (m_isRegistPhysicsWorld == true) {
			PhysicsWorld::GetInstance()->RemoveCollisionObject(m_ghostObject);
			m_isRegistPhysicsWorld = false;
		}
	}
	/*!
	* @brief	ボックス形状のゴーストオブジェクトを作成。
	*@param[in]	pos			座標。
	*@param[in]	rot			回転。
	*@param[in]	size		サイズ。
	*/
	void CreateBox(Vector3 pos, Quaternion rot, Vector3 size);
	/*!
	* @brief	カプセル形状のゴーストオブジェクトを作成。
	*@param[in]	pos			座標。
	*@param[in]	rot			回転。
	*@param[in]	radius		カプセルの半径。
	*@param[in]	height		カプセルの高さ。
	*/
	void CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height);
	/*!
	* @brief	球体形状のゴーストオブジェクトを作成。
	*@param[in]	pos			座標。
	*@param[in]	rot			回転。
	*@param[in]	radius		球体の半径。
	*/
	void CreateSphere(Vector3 pos, Quaternion rot, float radius);
	/*!
	* @brief	メッシュ形状のゴーストオブジェクトを作成。
	*@param[in]	pos					座標。
	*@param[in]	rot					回転。
	*@param[in]	skinModelData		スキンモデルデータ。
	*/
	void CreateMesh(Vector3 pos,Quaternion rot, const Model& model, const Matrix& worldMatrix);
	/*!
	* @brief	引数で渡されたゴーストオブジェクトが自分自身かどうか判定。
	*/
	bool IsSelf(const btCollisionObject& ghost) const
	{
		return &ghost == &m_ghostObject;
	}
	/*!
	* @brief	座標を設定。
	*/
	void SetPosition(const Vector3& pos)
	{
		auto& btTrans = m_ghostObject.getWorldTransform();
		btVector3 btPos;
		btPos = btVector3(pos.x, pos.y, pos.z);
		btTrans.setOrigin(btPos);
	}
	/*!
	* @brief	回転を設定。
	*/
	void SetRotation(const Quaternion& rot)
	{
		auto& btTrans = m_ghostObject.getWorldTransform();
		btQuaternion btRot;
		btRot = btQuaternion(rot.x, rot.y, rot.z, rot.w);
		btTrans.setRotation(btRot);
	}
private:
	/*!
	* @brief	ゴースト作成処理の共通処理。
	*/
	void CreateCommon(Vector3 pos, Quaternion rot);
private:
	bool							m_isRegistPhysicsWorld = false;	//物理ワールドに登録しているかどうかのフラグ。
	btGhostObject					m_ghostObject;					//ゴースト
	std::unique_ptr<ICollider>		m_collider;						//コライダー。
};

