#pragma once

class Fade;

class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	SpriteRender		m_spriteRender;			//スプライトレンダ―。
	bool				m_isWaitFadeout = false;
	Fade*				m_fade = nullptr;
};

