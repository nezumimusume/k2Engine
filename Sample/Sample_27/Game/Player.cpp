#include "stdafx.h"
#include "Player.h"

Player::Player()
{

}

Player::~Player()
{

}

bool Player::Start()
{
	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		100.0f,			//高さ。
		m_position		//座標。
	);

	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	m_spotLight.Init();

	//スポットライトを初期化。
	m_spotLight.Init();
	//ライトの有効距離を設定。
	m_spotLight.SetRange(300.0f);
	//角度を設定。
	m_spotLight.SetAngle(Math::DegToRad(20.0f));
	//距離による、光の減衰率を設定。
	m_spotLight.SetRangeAffectPowParam(1.5f);
	
	//ライトの色を設定。
	m_spotLight.SetColor(Vector3::One);
	//ライトの座標を設定。
	m_spotLight.SetPosition(m_position + Vector3(0.0f, 30.0f, 0.0f));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	//ライトの方向を設定。
	m_spotLight.SetDirection(m_forward);

	//ボリュームライトをスポットライトで初期化。
	m_volumeSpotLight.Init(m_spotLight);
	return true;
}

void Player::Update()
{
	//移動処理。
	Move();
	//回転処理。
	Rotation();

	//モデルを更新。
	m_modelRender.Update();

	//スポットライトを更新。
	//m_spotLight.Update();
	//ボリュームライトを更新。
	//m_volumeSpotLight.Update();
}

void Player::Move()
{
	//このフレームの移動量を求める。
	//左スティックの入力量を受け取る。
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();
	//カメラの前方方向と右方向を取得。
	Vector3 cameraForward = g_camera3D->GetForward();
	Vector3 cameraRight = g_camera3D->GetRight();
	//XZ平面での前方方向、右方向に変換する。
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();
	//XZ成分の移動速度をクリア。
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	m_moveSpeed += cameraForward * lStick_y * 200.0f;	//奥方向への移動速度を加算。
	m_moveSpeed += cameraRight * lStick_x * 200.0f;		//右方向への移動速度を加算。
	if (g_pad[0]->IsTrigger(enButtonA) //Aボタンが押されたら
		&& m_charaCon.IsOnGround()  //かつ、地面に居たら
		) {
		//ジャンプする。
		m_moveSpeed.y = 400.0f;	//上方向に速度を設定して、
	}
	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();
	//キャラクターコントローラーを使用して、座標を更新。

	m_position = m_charaCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
	if (m_charaCon.IsOnGround()) {
		//地面についた。
		m_moveSpeed.y = 0.0f;
	}

	//座標を設定。
	m_modelRender.SetPosition(m_position);

	//ライトの座標を設定。
	m_spotLight.SetPosition(m_position + Vector3(0.0f,30.0f,0.0f));
}

void Player::Rotation()
{
	if (fabsf(m_moveSpeed.x) >= 0.01f ||
		fabsf(m_moveSpeed.z) >= 0.01f)
	{
		m_rotation.SetRotationYFromDirectionXZ(m_moveSpeed);
		m_modelRender.SetRotation(m_rotation);
		m_forward = Vector3::AxisZ;
		m_rotation.Apply(m_forward);

		//ライトの方向を設定。
		m_spotLight.SetDirection(m_forward);
	}

}

void Player::Render(RenderContext& rc)
{
	// m_modelRender.Draw(rc);
}