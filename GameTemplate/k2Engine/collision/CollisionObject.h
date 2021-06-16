#pragma once
#include "physics/PhysicsGhostObject.h"

/// <summary>
/// 他のコリジョンと当たり判定を行うクラス。
/// </summary>
class CollisionObject : public IGameObject
{
public:
	CollisionObject();
	~CollisionObject();
	bool Start();
	void Update();
	/// <summary>
/// ボックス形状のゴーストオブジェクトを作成。
/// </summary>
/// <param name="pos">座標。</param>
/// <param name="rot">回転。</param>
/// <param name="size">サイズ。</param>
	void CreateBox(Vector3 pos, Quaternion rot, Vector3 size)
	{
		m_physicsGhostObject.CreateBox(pos, rot, size);
	}
	/// <summary>
	/// カプセル形状のゴーストオブジェクトを作成。
	/// </summary>
	/// <param name="pos">座標。</param>
	/// <param name="rot">回転。</param>
	/// <param name="radius">カプセルの半径。</param>
	/// <param name="height">カプセルの高さ。</param>
	void CreateCapsule(Vector3 pos, Quaternion rot, float radius, float height)
	{
		m_physicsGhostObject.CreateCapsule(pos, rot, radius, height);
	}
	/// <summary>
	/// 球形状のゴーストオブジェクトを作成。
	/// </summary>
	/// <param name="pos">座標。</param>
	/// <param name="rot">回転。</param>
	/// <param name="radius">球形の半径。</param>
	void CreateSphere(Vector3 pos, Quaternion rot, float radius)
	{
		m_physicsGhostObject.CreateSphere(pos, rot, radius);
	}
	/// <summary>
	/// メッシュ形状のゴーストオブジェクトを作成。
	/// </summary>
	/// <param name="pos">座標。</param>
	/// <param name="rot">回転。</param>
	/// <param name="model">モデル。</param>
	/// <param name="worldMatrix">ワールド行列。</param>
	void CreateMesh(Vector3 pos, Quaternion rot, const Model& model, const Matrix& worldMatrix)
	{
		m_physicsGhostObject.CreateMesh(pos, rot, model, worldMatrix);
	}
	/// <summary>
	/// 設定した時間(秒)が経過すると自動で削除されます。0.0fを設定したら1フレーム経過で削除されます。
	/// </summary>
	/// <param name="timeLimit">削除したい時間(秒)。</param>
	void SetTimeLimit(const float timeLimit)
	{
		m_timeLimit = timeLimit;
	}
	/// <summary>
	/// 名前を設定。
	/// </summary>
	/// <param name="name">名前。</param>
	void SetName(const char* name)
	{
		m_name = name;
	}
	/// <summary>
	/// 名前を取得。
	/// </summary>
	/// <returns></returns>
	const char* GetName() const
	{
		return m_name;
	}
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_physicsGhostObject.SetPosition(m_position);
	}
	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// 自動で削除するかどうかを設定する。
	/// </summary>
	/// <param name="isFlag">trueなら自動で削除される。falseなら自動で削除されないので、DeleteGOをする必要がある。</param>
	void SetIsEnableAutoDelete(bool isFlag)
	{
		m_isEnableAutoDelete = isFlag;
	}
	/// <summary>
	/// コリジョンオブジェクト同士の当たり判定。
	/// </summary>
	/// <param name="collisionObject">衝突判定したいコリジョンオブジェクト。</param>
	/// <returns>衝突したらtrue。</returns>
	bool GetIsCollision(CollisionObject& collisionObject) 
	{
		bool isCollision = false;
		PhysicsWorld::GetInstance()->ContactTest(&collisionObject.GetbtCollisionObject(), [&](const btCollisionObject& contactObject) {
			if (m_physicsGhostObject.IsSelf(contactObject) == true) {
				isCollision = true;
			}
		});
		return isCollision;
	}
	/// <summary>
	/// コリジョンオブジェクトとキャラコンの当たり判定。
	/// </summary>
	/// <param name="collisionObject">衝突判定したいキャラコン。</param>
	/// <returns>衝突したらtrue。</returns>
	bool GetIsCollision(CharacterController& characterController)
	{
		bool isCollision = false;
		PhysicsWorld::GetInstance()->ContactTest(characterController, [&](const btCollisionObject& contactObject) {
			if (m_physicsGhostObject.IsSelf(contactObject) == true) {
				isCollision = true;
			}
		});
		return isCollision;
	}
	/// <summary>
	/// ゴーストオブジェクトを取得。
	/// </summary>
	/// <returns>ゴーストオブジェクト。</returns>
	PhysicsGhostObject& GetGhostObject()
	{
		return m_physicsGhostObject;
	}
	/// <summary>
	/// btコリジョンオブジェクトを取得。
	/// </summary>
	/// <returns>btコリジョンオブジェクト。</returns>
	btCollisionObject& GetbtCollisionObject()
	{
		return m_physicsGhostObject.GetbtCollisionObject();
	}
private:
	Vector3						m_position;
	PhysicsGhostObject			m_physicsGhostObject;
	const char*					m_name = nullptr;
	float						m_timer = 0.0f;
	float						m_timeLimit = 0.0f;
	bool						m_isEnableAutoDelete = true;
};

class CollisionObjectManager
{
public:
	CollisionObjectManager();
	~CollisionObjectManager();
	void AddCollisionObject(CollisionObject* collisionObject)
	{
		m_collisionObjectVector.push_back(collisionObject);
	}
	/// <summary>
	/// 名前が完全一致するコリジョンオブジェクトを検索する。こちらは1つだけ。
	/// </summary>
	/// <param name="name">名前。</param>
	/// <returns>コリジョンオブジェクト。見つからなかった場合はnullptr。</returns>
	CollisionObject* FindCollisionObject(const char* name)
	{
		for (auto collisionObject : m_collisionObjectVector)
		{
			//名前一致！
			if (strcmp(collisionObject->GetName(), name) == 0)
			{
				return collisionObject;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// 名前が前方一致するコリジョンオブジェクトを検索する。こちらは1つだけ。
	/// </summary>
	/// <param name="name">名前。</param>
	/// <returns>コリジョンオブジェクト。見つからなかった場合はnullptr。</returns>
	CollisionObject* FindMatchForwardNameCollisionObject(const char* name)
	{
		for (auto collisionObject : m_collisionObjectVector)
		{
			auto len = strlen(name);
			auto namelen = strlen(collisionObject->GetName());
			if (len > namelen) {
				//名前が長い。不一致。
				continue;
			}
			if (strncmp(name, collisionObject->GetName(), len) == 0)
			{
				return collisionObject;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// 名前が完全一致するコリジョンオブジェクトを検索する。こちらは複数。
	/// </summary>
	/// <param name="name">名前。</param>
	/// <returns>コリジョンオブジェクトの配列。</returns>
	std::vector<CollisionObject*>& FindCollisionObjects(const char* name)
	{
		static std::vector<CollisionObject*> collisionObjectVector;
		collisionObjectVector.clear();
		for (auto collisionObject : m_collisionObjectVector)
		{
			//名前一致！
			if (strcmp(collisionObject->GetName(), name) == 0)
			{
				collisionObjectVector.push_back(collisionObject);
			}
		}
		return collisionObjectVector;
	}
	/// <summary>
	/// 名前が前方一致するコリジョンオブジェクトを検索する。こちらは複数。
	/// </summary>
	/// <param name="name">名前。</param>
	/// <returns>コリジョンオブジェクトの配列。</returns>
	std::vector<CollisionObject*>& FindMatchForwardNameCollisionObjects(const char* name)
	{
		static std::vector<CollisionObject*> collisionObjectVector;
		collisionObjectVector.clear();
		for (auto collisionObject : m_collisionObjectVector)
		{
			auto len = strlen(name);
			auto namelen = strlen(collisionObject->GetName());
			if (len > namelen) {
				//名前が長い。不一致。
				continue;
			}
			if (strncmp(name, collisionObject->GetName(), len) == 0)
			{
				collisionObjectVector.push_back(collisionObject);
			}
		}
		return collisionObjectVector;
	}
	/// <summary>
	/// 配列からコリジョンオブジェクトを削除。
	/// </summary>
	/// <param name="deleteCollisionObject">削除したいコリジョンオブジェクト。</param>
	void RemoveCollisionObject(CollisionObject* deleteCollisionObject)
	{
		for (auto it = m_collisionObjectVector.begin(); it != m_collisionObjectVector.end();) {
			// 条件一致した要素を削除する
			if (*it == deleteCollisionObject) {
				// 削除された要素の次を指すイテレータが返される。
				it = m_collisionObjectVector.erase(it);
			}
			// 要素削除をしない場合に、イテレータを進める
			else {
				++it;
			}
		}
	}
private:
	std::vector<CollisionObject*>		m_collisionObjectVector;

};
extern CollisionObjectManager* g_collisionObjectManager;