#pragma once

//クラス宣言。
class Player;

class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

	Player* m_player = nullptr;		//プレイヤー。
};

