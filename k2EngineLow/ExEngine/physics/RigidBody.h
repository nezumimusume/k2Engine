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
private:
	std::unique_ptr<btRigidBody>			m_rigidBody;		//剛体。
	std::unique_ptr<btDefaultMotionState>	m_myMotionState;	//モーションステート。
};
