#pragma once

//ばねカメラを使用したい場合は、SpringCameraをインクルードする。
#include "camera/SpringCamera.h"

//クラス宣言。
class Player;

class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();
	void UpdatePositionAndTarget();
	void ReStart();
	void NotifyGameClear();
private:
	Player*			m_player = nullptr;				//プレイヤー。
	Vector3			m_toCameraPos = Vector3::One;	//注視点から視点に向かうベクトル。
	SpringCamera	m_springCamera;					//ばねカメラ。
};

