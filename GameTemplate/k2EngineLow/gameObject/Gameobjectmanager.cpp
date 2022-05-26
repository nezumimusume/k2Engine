/*!
 *@brief	CGameObjectのマネージャ
 */
#include "k2EngineLowPreCompile.h"
#include "GameObjectManager.h"

namespace nsK2EngineLow {
	GameObjectManager* GameObjectManager::m_instance = nullptr;

	GameObjectManager::GameObjectManager()
	{
		if (m_instance != nullptr) {
			//インスタンスがすでに作られている。
			std::abort();
		}
		m_instance = this;
	}
	GameObjectManager::~GameObjectManager()
	{
		DeleteAllGameObjects();
		m_instance = nullptr;
	}
	void GameObjectManager::ExecuteUpdate()
	{
		//死亡フラグがついているゲームオブジェクトを破棄する。
		for (auto& goList : m_gameObjectListArray) {
			goList.remove_if([&](IGameObject* go) {
				if (go->IsDead()) {
					delete go;
					return true;
				}
				return false;
			});
		}

		for (auto& goList : m_gameObjectListArray) {
			for (auto& go : goList) {
				go->StartWrapper();
			}
		}

		for (auto& goList : m_gameObjectListArray) {
			for (auto& go : goList) {
				go->UpdateWrapper();
			}
		}
		
		//物理エンジンのアップデートを呼び出す。
		PhysicsWorld::GetInstance()->Update(1.0f / 60.0f);
	}
	void GameObjectManager::ExecuteRender(RenderContext& rc)
	{
		for (auto& goList : m_gameObjectListArray) {
			for (auto& go : goList) {
				go->RenderWrapper(rc);
			}
		}
	}
}