#pragma once

//�N���X�錾�B
class Fade;

/// <summary>
/// �^�C�g���B
/// </summary>
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	SpriteRender			m_spriteRender;					//�^�C�g���̉摜�B
	float					m_alpha = 0.0f;					//pressbutton�̃��l�B
	SpriteRender			m_pressButton;					//pressbutton�̉摜�B
	SoundSource*			m_bgm = nullptr;				//�^�C�g����BGM�B
	Fade*					m_fade = nullptr;				//�t�F�[�h�B
	bool					m_isWaitFadeout = false;
};

