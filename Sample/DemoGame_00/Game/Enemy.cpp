#include "stdafx.h"
#include "Enemy.h"

#include "MovedPath.h"
#include "Player.h"
#include "Game.h"

namespace
{
	const float MOVE_SPPED = 200.0f;
	const float SERACH_PLAYER_DISTANCE = 300.0f;
	const float SERACH_PLAYER_ANGLE = 0.2f;
}


Enemy::Enemy()
{

}

Enemy::~Enemy()
{

}

bool Enemy::Start()
{
	//アニメーションクリップをロードする。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/enemy/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/enemy/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	//エネミーのモデルを読み込む。
	m_modelRender.Init("Assets/modelData/enemy.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisZ);
	//座標を大きさを設定。
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);

	m_movedPath->Init(m_position, MOVE_SPPED);
#ifdef USE_VOLUME_SPOTLIGHT
	m_spotLight.Init();
	m_volumeSpotLight.Init(m_spotLight);
#endif
	return true;
}

void Enemy::LoadPath(const int number)
{
	m_movedPath = std::make_unique<MovedPath>();
	char wcsbuf[256];
	sprintf_s(wcsbuf, 256, "Assets/path/path_enemy%d.tkl", int(number));
	m_movedPath->ReadPath(wcsbuf);
}

void Enemy::Update()
{
	if (GetPlayer() == false)
	{
		return;
	}
	Vector3 pos;
	pos = m_position;
	pos.y += 50.0f;
#ifdef USE_VOLUME_SPOTLIGHT
	m_spotLight.SetPosition(pos);
	m_spotLight.SetColor(10.0f, 10.0f, 10.0f);
	m_spotLight.SetDirection(m_moveVector);
	m_spotLight.SetAngle(Math::DegToRad(15.0f));
	m_spotLight.SetRange(400.0f);

	m_spotLight.SetRangeAffectPowParam(2.0f);
	m_spotLight.Update();
	m_volumeSpotLight.Update();
#endif
	//移動処理。
	Move();
	//旋回処理。
	Turn();
	//ステート管理。
	ManageState();
	//アニメーション再生。
	PlayAnimation();
	//プレイヤーを探索。
	SearchPlayer();

	//モデルを更新。
	m_modelRender.Update();
}

void Enemy::Move()
{
	//プレイヤーがゲームオーバーかゲームクリアなら移動させない。
	if (m_player->GetPlayerState() == Player::enPlayerState_GameClear ||
		m_player->GetPlayerState() == Player::enPlayerState_GameOver ||
		m_player->GetPlayerState() == Player::enPlayerState_GameClear_Idle)
	{
		return;
	}

	m_position = m_movedPath->Move();
	m_moveVector = m_movedPath->GetMoveVector();
	//座標を設定。
	m_modelRender.SetPosition(m_position);
}

void Enemy::Turn()
{
	//atan2はtanθの値を角度(ラジアン単位)に変換してくれる関数。
	//m_moveVector.x / m_moveVector.zの結果はtanθになる。
	//atan2を使用して、角度を求めている。
	//これが回転角度になる。
	float angle = atan2(-m_moveVector.x, m_moveVector.z);
	//atanが返してくる角度はラジアン単位なので
	//SetRotationDegではなくSetRotationを使用する。
	m_rotation.SetRotationY(-angle);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
}

void Enemy::ManageState()
{
	//停止中なら。
	if (m_movedPath->GetIsStop())
	{
		//ステートを待機中にする。
		m_enemyState = enEnemyState_Idle;
	}
	//移動中なら。
	else
	{
		//ステートを歩きにする。
		m_enemyState = enEnemyState_Run;
	}
}

void Enemy::PlayAnimation()
{
	//switch文。
	switch (m_enemyState) {
		//プレイヤーステートが待機だったら。
	case enEnemyState_Idle:
		//待機アニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//プレイヤーステートが走りだったら。
	case enEnemyState_Run:
		//走りアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
	}
}

void Enemy::SearchPlayer()
{
	//プレイヤーがゲームオーバーかゲームクリアなら移動させない。
	if (m_player->GetPlayerState() == Player::enPlayerState_GameClear ||
		m_player->GetPlayerState() == Player::enPlayerState_GameOver ||
		m_player->GetPlayerState() == Player::enPlayerState_GameClear_Idle)
	{
		return;
	}

	//敵からプレイヤーに向かうベクトルを計算。
	Vector3 toPlayer = m_player->GetPosition() - m_position;
	if (toPlayer.Length() < SERACH_PLAYER_DISTANCE) {
		//視野角を判定。
		toPlayer.Normalize();

		float angle = acosf(toPlayer.Dot(m_moveVector));
		if (fabsf(angle) < Math::PI * SERACH_PLAYER_ANGLE) {
			//視野角45度以内。
			//ゲームにゲームオーバーを通知。
			Game* game = FindGO<Game>("game");
			game->NotifyGameOver();
		}
	}
}

void Enemy::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}