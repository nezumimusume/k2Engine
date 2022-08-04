#pragma once

//ばねカメラを使用したい場合は、SpringCameraをインクルードする。
#include "camera/SpringCamera.h"

//クラス宣言。
class Dancer;

class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

	/////////////////////////////////////
	//メンバ変数
	/////////////////////////////////////
	Dancer* m_dancer = nullptr;		//プレイヤー。
	Vector3 m_toCameraPos = Vector3::One;
	SpringCamera m_springCamera;	//ばねカメラ。
};

