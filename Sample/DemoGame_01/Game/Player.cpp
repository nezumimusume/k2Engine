#include "stdafx.h"
#include "Player.h"

#include "Game.h"
#include "FireBall.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

Player::Player()
{

}

Player::~Player()
{

}

bool Player::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/human/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/human/walk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/human/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/human/attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_MagicAttack].Load("Assets/animData/human/magicattack.tka");
	m_animationClips[enAnimationClip_MagicAttack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/human/receivedamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/human/down.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);
	m_animationClips[enAnimationClip_PushLever].Load("Assets/animData/human/push_lever.tka");
	m_animationClips[enAnimationClip_PushLever].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/human/human.tkm",m_animationClips,enAnimationClip_Num);
	//m_modelRender.PlayAnimation(enAnimationClip_Run);
	//m_modelRender.Init("Assets/modelData/human.tkm");
	m_modelRender.SetPosition(m_position);


	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		100.0f,			//高さ。
		m_position		//座標。
	);

	m_swordBoneId = m_modelRender.FindBoneID(L"Sword");

	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
	});

	EffectEngine::GetInstance()->ResistEffect(1, u"Assets/effect/efk/enemy_slash_01.efk");
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/magic.wav");
	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/slash.wav");
	return true;
}

void Player::Update()
{
	//移動処理。
	Move();
	//回転処理。
	Rotation();
	//当たり判定。
	Collision();
	//攻撃処理。
	Attack();
	//アニメーションの再生。
	PlayAnimation();
	//ステートの遷移処理。
	ManageState();


	//モデルの更新。
	m_modelRender.Update();
}

void Player::Move()
{
	//移動できない状態であれば、移動処理はしない。
	if (GetIsEnableMove() == false)
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
	m_moveSpeed += cameraForward * lStick_y * 250.0f;	//奥方向への移動速度を加算。
	m_moveSpeed += cameraRight * lStick_x * 250.0f;		//右方向への移動速度を加算。
	//現在y軸移動は無し。
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
	//ちょっとだけモデルの座標を挙げる。
	modelPosition.y += 2.5f;
	m_modelRender.SetPosition(modelPosition);
}

void Player::Rotation()
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

	//プレイヤーの前ベクトルを計算する。
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Player::Collision()
{
	if (m_playerState == enPlayerState_ReceiveDamage ||
		m_playerState == enPlayerState_Down ||
		m_playerState == enPlayerState_GameOver)
	{
		return;
	}

	{
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_attack");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charaCon))
			{
				m_hp -= 1;
				if (m_hp <= 0)
				{
					m_playerState = enPlayerState_Down;
				}
				else {
					m_playerState = enPlayerState_ReceiveDamage;
				}
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.4f);
				return;
			}
		}
	}

	{
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_fireball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charaCon))
			{
				m_hp -= 1;
				if (m_hp <= 0)
				{
					m_playerState = enPlayerState_Down;
				}
				else {
					m_playerState = enPlayerState_ReceiveDamage;
				}
				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(4);
				se->Play(false);
				se->SetVolume(0.6f);
				return;
			}
		}
	}
}

void Player::Attack()
{
	if (m_playerState != enPlayerState_Attack)
	{
		return;
	}

	//攻撃判定中であれば。
	if (m_isUnderAttack == true)
	{
		//攻撃用のコリジョンを作成する。
		MakeAttackCollision();
	}
}

void Player::MakeAttackCollision()
{
	auto collisionObject = NewGO<CollisionObject>(0);
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(100.0f, 10.0f, 10.0f));
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("player_attack");
}

void Player::MakeFireBall()
{
	FireBall* fireBall = NewGO<FireBall>(0);
	Vector3 fireBallPosition = m_position;
	fireBallPosition.y += 70.0f;
	fireBall->SetPosition(fireBallPosition);
	fireBall->SetRotation(m_rotation);
}

void Player::MakePushLeverCollision()
{
	auto collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPosition = m_position;
	//座標をプレイヤーの前に設定する。
	collisionPosition += m_forward * 50.0f;
	//collisionPosition.y += 70.0f;
	collisionObject->CreateSphere(collisionPosition, Quaternion::Identity, 70.0f);
	collisionObject->SetName("player_lever");
}

void Player::ProcessCommonStateTranstion()
{
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		if (m_moveSpeed.LengthSq() >= 200.0f * 200.0f)
		{
			//ステートを走りにする。
			m_playerState = enPlayerState_Run;
		}
		else
		{
			//歩きにする。
			m_playerState = enPlayerState_Walk;
		}

	}
	//xとzの移動速度が無かったら(スティックの入力が無かったら)。
	else
	{
		//ステートを待機にする。
		m_playerState = enPlayerState_Idle;
	}

	if (g_pad[0]->IsTrigger(enButtonY))
	{
		m_playerState = enPlayerState_Attack;
		m_isUnderAttack = false;
	}
	else if (g_pad[0]->IsTrigger(enButtonX))
	{
		m_playerState = enPlayerState_MagicAttack;
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(0);
		se->SetVolume(0.5f);
		se->Play(false);
	}
	else if (g_pad[0]->IsTrigger(enButtonA))
	{
		m_playerState = enPlayerState_PushLever;
		MakePushLeverCollision();
	}
}

void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTranstion();
}

void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTranstion();
}

void Player::ProcessRunStateTransition()
{
	ProcessCommonStateTranstion();
}

void Player::ProcessAttackStateTransition()
{
	//攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTranstion();
	}
}

void Player::ProcessMagicAttackStateTransition()
{
	//魔法攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTranstion();
	}
}

void Player::ProcessReceiveDamageStateTransition()
{
	//被ダメージアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTranstion();
	}
}

void Player::ProcessPushLeverStateTransition()
{
	//レバーを押すアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTranstion();
	}
}

void Player::ProcessDownStateTransition()
{
	//ダウンアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ゲームオーバーを通知する。
		Game* game = FindGO<Game>("game");
		game->NotifyGameOver();
		m_playerState = enPlayerState_GameOver;
	}
}

void Player::ManageState()
{
	switch (m_playerState)
	{
	case enPlayerState_Idle:
		ProcessIdleStateTransition();
		break;
	case enPlayerState_Walk:
		ProcessWalkStateTransition();
		break;
	case enPlayerState_Run:
		ProcessRunStateTransition();
		break;
	case enPlayerState_Attack:
		ProcessAttackStateTransition();
		break;
	case enPlayerState_MagicAttack:
		ProcessMagicAttackStateTransition();
		break;
	case enPlayerState_PushLever:
		ProcessPushLeverStateTransition();
		break;
	case enPlayerState_ReceiveDamage:
		ProcessReceiveDamageStateTransition();
		break;
	case enPlayerState_Down:
		ProcessDownStateTransition();
		break;
	}
}

void Player::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_playerState)
	{
	case enPlayerState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.5f);
		break;
	case enPlayerState_Walk:
		m_modelRender.SetAnimationSpeed(1.2f);
		m_modelRender.PlayAnimation(enAnimationClip_Walk, 0.1f);
		break;
	case enPlayerState_Run:
		m_modelRender.SetAnimationSpeed(1.2f);
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	case enPlayerState_Attack:
		m_modelRender.SetAnimationSpeed(1.5f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
	case enPlayerState_MagicAttack:
		m_modelRender.SetAnimationSpeed(1.2f);
		m_modelRender.PlayAnimation(enAnimationClip_MagicAttack, 0.1f);
		break;
	case enPlayerState_PushLever:
		m_modelRender.PlayAnimation(enAnimationClip_PushLever, 0.1f);
		break;
	case enPlayerState_ReceiveDamage:
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
	case enPlayerState_Down:
		m_modelRender.PlayAnimation(enAnimationClip_Down, 0.1f);
		break;
	case enPlayerState_GameOver:
		break;
	default:
		break;
	}
}

void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	if (wcscmp(eventName, L"attack_start") == 0) {
		m_isUnderAttack = true;
		//攻撃エフェクトを発生させる。
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(1);
		effectEmitter->SetScale(Vector3::One * 12.0f);
		Vector3 effectPosition = m_position;
		effectPosition.y += 70.0f;
		//effectPosition += m_forward * 10.0f;
		effectEmitter->SetPosition(effectPosition);
		Quaternion rotation;
		rotation = m_rotation;
		rotation.AddRotationDegY(360.0f);
		rotation.AddRotationDegZ(180.0f);
		effectEmitter->SetRotation(rotation);
		effectEmitter->Play();
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(3);
		se->Play(false);
		se->SetVolume(0.8f);
	}
	else if (wcscmp(eventName, L"attack_end") == 0) {
		m_isUnderAttack = false;
	}
	else if (wcscmp(eventName, L"magic_attack") == 0) {
		MakeFireBall();
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}