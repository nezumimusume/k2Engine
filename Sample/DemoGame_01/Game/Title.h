#pragma once

class Fade;
class SoundSource;

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
	SoundSource*			m_bgm = nullptr;
	Fade*					m_fade = nullptr;
	bool					m_isWaitFadeout = false;
};

