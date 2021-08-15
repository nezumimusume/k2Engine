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
	SpriteRender		m_spriteRender;			//�X�v���C�g�����_�\�B
	SpriteRender		m_pressA;				//PRESSA�{�^���摜�B
	bool				m_isWaitFadeout = false;
	Fade*				m_fade = nullptr;
	SoundSource*		m_bgm = nullptr;
};

