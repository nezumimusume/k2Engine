#pragma once
class BackGround : public IGameObject
{
public:
	BackGround()
	{
		modelRender.Init("Assets/modelData/bg/bg.tkm");
		modelRender.SetScale({ 3.0f, 3.0f, 3.0f });
		Quaternion rotation;
		rotation.SetRotationDegY(180.0f);
		modelRender.SetRotation(rotation);
	}
	~BackGround() {}

	void Update()
	{
		modelRender.Update();
		modelRender.Draw();
	}

	void Render(RenderContext& rc)
	{
	
	}

	ModelRender modelRender;

};

