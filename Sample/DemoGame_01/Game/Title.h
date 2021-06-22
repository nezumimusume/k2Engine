#pragma once
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	SpriteRender			m_spriteRender;
	float					m_alpha = 0.0f;
	SpriteRender			m_pressButton;
};

