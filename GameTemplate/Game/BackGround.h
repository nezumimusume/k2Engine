#pragma once

#include "physics/PhysicsGhostObject.h"

class BackGround : public IGameObject
{
public:
	BackGround()
	{
		modelRender.Init("Assets/modelData/bg/bg.tkm");
		//modelRender.Init("Assets/modelData/stage/stage.tkm");
		modelRender.SetScale({ 3.0f, 3.0f, 3.0f });
		Quaternion rotation;
		rotation.SetRotationDegY(180.0f);
		modelRender.SetRotation(rotation);
		modelRender.Update();
		//pso.CreateFromModel(modelRender.GetModel(), modelRender.GetModel().GetWorldMatrix());
		go.CreateBox(Vector3(100.0f, 100.0f, 100.0f), Quaternion::Identity, Vector3::One * 200.0f);
		PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	}
	~BackGround() {}

	void Update()
	{
		modelRender.Update();
	}

	void Render(RenderContext& rc)
	{
		modelRender.Draw(rc);
	}

	ModelRender modelRender;
	PhysicsStaticObject pso;
	PhysicsGhostObject go;
	
};

