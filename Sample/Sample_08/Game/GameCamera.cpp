#include "stdafx.h"
#include "GameCamera.h"

#include "Player.h"

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	//プレイヤーのインスタンスを探す。
	m_player = FindGO<Player>("player");

	return true;
}

void GameCamera::Update()
{
	//カメラを更新。
	//注視点を計算する。
	Vector3 target = m_player->m_position;
	//プレイヤの足元からちょっと上を注視点とする。
	target.y += 100.0f;
	//視点を計算する。
	//キャラクタを斜め上から見ているような視点にする。
	Vector3 toPos = { 0.0f, 50.0f, -300.0f };
	Vector3 pos = target + toPos;	
	//メインカメラに注視点と視点を設定する。
	g_camera3D->SetTarget(target);
	g_camera3D->SetPosition(pos);

	//カメラの更新。
	g_camera3D->Update();
}