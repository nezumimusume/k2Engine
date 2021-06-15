#include "stdafx.h"
#include "Enemy.h"

#include "collision/CollisionObject.h"

Enemy::Enemy()
{

}

Enemy::~Enemy()
{

}

bool Enemy::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/enemy/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/enemy/walk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/enemy/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/enemy/attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/enemy/damage_receive.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/enemy/down.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);

	m_modelRender.Init("Assets/modelData/enemy/enemy.tkm", m_animationClips, enAnimationClip_Num);
	//m_modelRender.PlayAnimation(enAnimationClip_Run);
	//m_modelRender.Init("Assets/modelData/enemy.tkm");
	m_modelRender.SetPosition(m_position);


	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		100.0f,			//高さ。
		m_position		//座標。
	);
	return true;
}

void Enemy::Update()
{
	//移動処理。
	Move();
	//回転処理。
	Rotation();
	//攻撃処理。
	Attack();
	//アニメーションの再生。
	PlayAnimation();
	//ステートの管理。
	ManageState();
	//当たり判定。
	Collision();

	m_modelRender.Update();
}

void Enemy::Move()
{
	if (m_EnemyState == 3)
	{
		return;
	}


	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

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
	//m_moveSpeed += cameraForward * lStick_y * 250.0f;	//奥方向への移動速度を加算。
	//m_moveSpeed += cameraRight * lStick_x * 250.0f;		//右方向への移動速度を加算。
	/*if (g_pad[0]->IsTrigger(enButtonA) //Aボタンが押されたら
		&& m_charaCon.IsOnGround()  //かつ、地面に居たら
		) {
		//ジャンプする。
		m_moveSpeed.y = 400.0f;	//上方向に速度を設定して、
	}
	*/

	//m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();
	//キャラクターコントローラーを使用して、座標を更新。

	m_position = m_charaCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
	if (m_charaCon.IsOnGround()) {
		//地面についた。
		m_moveSpeed.y = 0.0f;
	}
	Vector3 modelPosition = m_position;
	modelPosition.y += 2.5f;
	m_modelRender.SetPosition(modelPosition);
}

void Enemy::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		//m_moveSpeed.xとm_moveSpeed.zの絶対値がともに0.001以下ということは
		//このフレームではキャラは移動していないので旋回する必要はない。
		return;
	}
	//atan2はtanθの値を角度(ラジアン単位)に変換してくれる関数。
	//m_moveSpeed.x / m_moveSpeed.zの結果はtanθになる。
	//atan2を使用して、角度を求めている。
	//これが回転角度になる。
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	//atanが返してくる角度はラジアン単位なので
	//SetRotationDegではなくSetRotationを使用する。
	m_rotation.SetRotationY(-angle);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
}

void Enemy::Attack()
{
	if (m_EnemyState == 3)
	{
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			m_isAttack = false;
		}
		return;
	}


	if (g_pad[0]->IsTrigger(enButtonY))
	{
		m_isAttack = true;

	}
}

void Enemy::ManageState()
{
	//地面に付いていなかったら。
	/*if (m_charaCon.IsOnGround() == false)
	{
		m_EnemyState =
		return;
	}
	*/

	if (m_isAttack == true)
	{
		m_EnemyState = 3;
		return;
	}

	//地面に付いていたら。
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		if (m_moveSpeed.LengthSq() >= 200 * 200.0f)
		{
			//ステートを走りにする。
			m_EnemyState = 2;
		}
		else
		{
			//歩きにする。
			m_EnemyState = 1;
		}

	}
	//xとzの移動速度が無かったら(スティックの入力が無かったら)。
	else
	{
		//ステートを待機にする。
		m_EnemyState = 0;
	}
}

void Enemy::PlayAnimation()
{
	switch (m_EnemyState)
	{
	case 0:
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.5f);
		break;
	case 1:
		m_modelRender.PlayAnimation(enAnimationClip_Walk, 0.1f);
		break;
	case 2:
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	case 3:
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
	}
}

void Enemy::Collision()
{
	{
		auto collisions = g_collisionObjectManager->FindCollisionObjects("fire");

		for (auto collision : collisions)
		{
			if (collision->GetIsCollision(m_charaCon) == true)
			{
				DeleteGO(this);
			}
		}
	}

	{
		auto collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");

		for (auto collision : collisions)
		{
			if (collision->GetIsCollision(m_charaCon) == true)
			{
				DeleteGO(this);
			}
		}
	}
}

void Enemy::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}