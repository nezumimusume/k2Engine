#pragma once

class Player;
class BackGround;
class GameCamera;
class Dancer;
class GlowingObject;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	Player*						m_player = nullptr;
	BackGround*					m_backGround = nullptr;
	GameCamera*					m_gameCamera = nullptr;
	SpotLight					m_spotLights[4];
	Dancer*						m_dancer = nullptr;
	LevelRender					m_level;
	SoundSource*				m_bgm = nullptr;
	std::vector<GlowingObject*>	m_glowingObjectList;
};

