#pragma once
class Sphere : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	ModelRender		m_modelRender;
};

