#pragma once
class Star : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	Vector3 m_postiion;
	Vector3 m_scale = Vector3::One * 0.5f;
	ModelRender m_modelRender;
};

