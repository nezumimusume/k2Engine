#pragma once

class Player : public IGameObject
{
	bool Start() override
 	{
		m_modelRender.Init("Assets/modelData/unityChan.tkm");
		return true;
	}

	void Update() override
	{
		m_modelRender.Update();
	}

	void Render(RenderContext& rc) override
	{
		m_modelRender.Draw(rc);
	}
private:
	ModelRender m_modelRender;
};
