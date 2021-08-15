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
	SpriteRender		m_pressA;				//PRESSAボタン画像。
	bool				m_isWaitFadeout = false;
	Fade*				m_fade = nullptr;
	SoundSource*		m_bgm = nullptr;
};

