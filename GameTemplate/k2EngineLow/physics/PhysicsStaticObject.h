/*!
 * @brief	静的オブジェクト。
 */

#pragma once

#include "physics/MeshCollider.h"
#include "physics/RigidBody.h"

namespace nsK2EngineLow {
	/// <summary>
	/// 静的物理オブジェクト
	/// </summary>
	class PhysicsStaticObject : public Noncopyable {
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
		/// <param name="restitution">反発力</param>
		void CreateFromModel(Model& model, const Matrix& worldMatrix, const float restitution = 0.0f);
		/// <summary>
		/// 摩擦力を設定する。
		/// </summary>
		/// <param name="friction">摩擦力。10が最大値。</param>
		void SetFriction(float friction)
		{
			m_rigidBody.SetFriction(friction);
		}
		btCollisionObject* GetbtCollisionObject()
		{
			return m_rigidBody.GetBody();
		}
		/// <summary>
		/// 剛体を破棄。
		/// </summary>
		void Release()
		{
			m_rigidBody.Release();
		}
	private:
		MeshCollider m_meshCollider;		//メッシュコライダー。
		RigidBody m_rigidBody;				//剛体。
	};
}