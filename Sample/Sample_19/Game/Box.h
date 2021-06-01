#pragma once

//ゴーストオブジェクトを使用するには、ファイルをインクルードする必要がある。
#include "physics/PhysicsGhostObject.h"

class Box : public IGameObject
{
public:
	bool Start();
	
	//ゴーストオブジェクト。
	PhysicsGhostObject m_physicsGhostObject;
};

