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
	g_camera3D->SetPosition({ 0.0f, 100.0f, 400.0f });
	g_camera3D->SetFar(40000.0f);
	return true;
}


void GameCamera::Update()
{
	//カメラを更新。
	//注視点を計算する。

	//プレイヤの足元からちょっと上を注視点とする。
	Vector3 cameraPos = g_camera3D->GetPosition();
	Vector3 cameraPosOld = cameraPos;
	//パッドの入力を使ってカメラを回す。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y軸周りの回転
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 0.5f * x);
	qRot.Apply(cameraPos);
	//X軸周りの回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, cameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 0.5f * y);
	qRot.Apply(cameraPos);
	//カメラの回転の上限をチェックする。
	//注視点から視点までのベクトルを正規化する。
	//正規化すると、ベクトルの大きさが１になる。
	//大きさが１になるということは、ベクトルから強さがなくなり、方向のみの情報となるということ。
	Vector3 toPosDir = cameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//カメラが上向きすぎ。
		cameraPos = cameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//カメラが下向きすぎ。
		cameraPos = cameraPosOld;
	}

	g_camera3D->SetPosition(cameraPos);
}