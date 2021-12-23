#pragma once

class Player;
class BackGround;
class GameCamera;
class Lantern;

class Game : public IGameObject
{
public:
	Game() {}
	~Game() {}
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player*						m_player = nullptr;
	BackGround*					m_backGround = nullptr;
	GameCamera*					m_gameCamera = nullptr;
	Quaternion					m_rotation;
	std::vector<Lantern*>		m_lanternList;
	LevelRender					m_levelRender;
};

