#pragma once

//クラス宣言。
class Fade;

/// <summary>
/// タイトル。
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
	SpriteRender			m_spriteRender;					//タイトルの画像。
	float					m_alpha = 0.0f;					//pressbuttonのα値。
	SpriteRender			m_pressButton;					//pressbuttonの画像。
	SoundSource*			m_bgm = nullptr;				//タイトルのBGM。
	Fade*					m_fade = nullptr;				//フェード。
	bool					m_isWaitFadeout = false;
};

