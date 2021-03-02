/*!
 * @brief	静的オブジェクト。
 */

#pragma once

#include "physics/MeshCollider.h"
#include "physics/RigidBody.h"


/// <summary>
/// 静的物理オブジェクト
/// </summary>
class PhysicsStaticObject{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PhysicsStaticObject();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~PhysicsStaticObject();
	/// <summary>
	/// モデルからの静的オブジェクトの作成。
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldMatrix">ワールド行列</param>
	void CreateFromModel(Model& model, const Matrix& worldMatrix);
private:
	MeshCollider m_meshCollider;		//メッシュコライダー。
	RigidBody m_rigidBody;				//剛体。
};
