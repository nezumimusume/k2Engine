#include "stdafx.h"
#include "Boss.h"


#include "Game.h"
#include "FireBall.h"
#include "Player.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

#include <time.h>
#include <stdlib.h>

Boss::Boss()
{

}

Boss::~Boss()
{

}

bool Boss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/enemy/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/enemy/walk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/enemy/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/enemy/attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_MagicAttack].Load("Assets/animData/enemy/magicattack.tka");
	m_animationClips[enAnimationClip_MagicAttack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/enemy/receivedamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/enemy/down.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);
	m_modelRender.Init("Assets/modelData/boss/boss.tkm", m_animationClips, enAnimationClip_Num);
	m_modelRender.SetPosition(m_position);
	m_spawnPosition = m_position;
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

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
	EffectEngine::GetInstance()->ResistEffect(2, u"Assets/effect/efk/cast_fire.efk");
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/magic.wav");
	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/slash.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/hit.wav");

	m_player = FindGO<Player>("player");
	//乱数を初期化。
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	return true;
}

void Boss::Update()
{
	//追跡処理。
	Chase();
	//回転処理。
	Rotation();
	//当たり判定。
	Collision();
	//攻撃処理。
	Attack();
	//被ダメージ。
	ReceiveDamage();
	//ダウン。
	Down();
	//アニメーションの再生。
	PlayAnimation();
	//ステートの遷移処理。
	ManageState();



	//モデルの更新。
	m_modelRender.Update();
}

void Boss::Rotation()
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

void Boss::Chase()
{
	//追跡ステートでないなら、追跡処理はしない。
	if (m_BossState != enBossState_Chase)
	{
		return;
	}

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

void Boss::Collision()
{
	if (m_BossState == enBossState_ReceiveDamage ||
		m_BossState == enBossState_Down)
	{
		return;
	}

	{
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charaCon))
			{
				m_hp -= 1;
				if (m_hp <= 0)
				{
					m_BossState = enBossState_Down;
				}
				else {
					m_BossState = enBossState_ReceiveDamage;
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
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_fireball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charaCon))
			{
				m_hp -= 1;
				if (m_hp <= 0)
				{
					m_BossState = enBossState_Down;
				}
				else {
					m_BossState = enBossState_ReceiveDamage;
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

void Boss::Attack()
{
	if (m_BossState != enBossState_Attack)
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

const bool Boss::SearchPlayer() const
{
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーにある程度近かったら.。
	if (diff.LengthSq() <= 700.0 * 700.0f)
	{
		//エネミーからプレイヤーに向かうベクトルを正規化する。
		diff.Normalize();
		//エネミーの正面のベクトルと、エネミーからプレイヤーに向かうベクトルの。
		//内積(cosθ)を求める。
		float cos = m_forward.Dot(diff);
		//内積(cosθ)から角度(θ)を求める。
		float angle = acosf(cos);
		//角度(θ)が180°より小さければ。
		if (angle <= (Math::PI / 180.0f) * 180.0f)
		{
			//プレイヤーを見つけた！
			return true;
		}
	}

	return false;
}

void Boss::ReceiveDamage()
{
	if (m_BossState != enBossState_ReceiveDamage)
	{
		return;
	}
}

void Boss::Down()
{
	if (m_BossState != enBossState_Down)
	{
		return;
	}
}

void Boss::MakeAttackCollision()
{
	auto collisionObject = NewGO<CollisionObject>(0);
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	collisionObject->CreateBox(m_position, Quaternion::Identity, Vector3(100.0f, 10.0f, 10.0f));
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("enemy_attack");
}

void Boss::MakeFireBall()
{
	FireBall* fireBall = NewGO<FireBall>(0);
	Vector3 fireBallPosition = m_position;
	fireBallPosition.y += 70.0f;
	fireBall->SetPosition(fireBallPosition);
	fireBall->SetRotation(m_rotation);
	fireBall->SetEnMagician(FireBall::enMagician_Enemy);
}

void Boss::ProcessCommonStateTransition()
{
	//各タイマーを初期化。
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	//まだ途中。
	{
		Vector3 diff = m_spawnPosition - m_position;

	}
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーを見つけたら。
	if (SearchPlayer())
	{
		diff.Normalize();
		//移動速度を設定する。
		m_moveSpeed = diff * 320.0f;
		//攻撃できる距離なら。
		if (IsCanAttack())
		{
			//乱数によって、攻撃するか待機させるかを決定する。	
			int ram = rand() % 100;
			if (ram > 30)
			{
				//攻撃ステートに移行する。
				m_BossState = enBossState_Attack;
				m_isUnderAttack = false;
				return;
			}
			else
			{
				//待機ステートに移行する。
				m_BossState = enBossState_Idle;
				return;
			}

		}
		//攻撃できない距離なら。
		else
		{
			//乱数によって、追跡させるか魔法攻撃をするか決定する。	
			int ram = rand() % 100;
			if (ram > 40)
			{
				//追跡ステートに移行する。
				m_BossState = enBossState_Chase;
				return;
			}
			else {
				//現在が魔法攻撃ステートなら。
				//連続で魔法を撃たせないようにする。
				if (m_BossState == enBossState_MagicAttack)
				{
					//追跡ステートに移行する。
					m_BossState = enBossState_Chase;
					return;
				}
				else
				{
					//魔法攻撃ステートに移行する。
					m_BossState = enBossState_MagicAttack;
					EffectEmitter* effect = NewGO<EffectEmitter>(0);
					effect->Init(2);
					Vector3 effectPosition = m_position;
					effect->SetPosition(m_position);
					effect->SetScale(Vector3::One * 10.0f);
					effect->Play();
					return;
				}
			}
		}
	}
	//プレイヤーを見つけられなければ。
	else
	{
		//待機ステートに移行する。
		m_BossState = enBossState_Idle;
		return;

	}
}

void Boss::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//待機時間がある程度経過したら。
	if (m_idleTimer >= 0.5f)
	{
		//他のステートへ移行する。
		ProcessCommonStateTransition();
	}

}

void Boss::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Boss::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void Boss::ProcessChaseStateTransition()
{
	//攻撃できる距離なら。
	if (IsCanAttack())
	{
		//追跡ステートから他のステートへ移行する。
		ProcessCommonStateTransition();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	//追跡時間がある程度経過したら。
	if (m_chaseTimer >= 0.5f)
	{
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessAttackStateTransition()
{
	//攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessMagicAttackStateTransition()
{
	//魔法攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessReceiveDamageStateTransition()
{
	//被ダメージアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//攻撃されたら距離関係無しに、取り敢えず追跡させる。
		m_BossState = enBossState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		//移動速度を設定する。
		m_moveSpeed = diff * 150.0f;
	}
}

void Boss::ProcessDownStateTransition()
{
	//ダウンアニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		Game* game = FindGO<Game>("game");
		game->AddDefeatedEnemyNumber();
		DeleteGO(this);
	}
}

void Boss::ManageState()
{
	switch (m_BossState)
	{
	case enBossState_Idle:
		ProcessIdleStateTransition();
		break;
		/*case enBossState_Walk:
			ProcessWalkStateTransition();
			break;
		case enBossState_Run:
			ProcessRunStateTransition();
			break;
		*/
	case enBossState_Chase:
		ProcessChaseStateTransition();
		break;
	case enBossState_Attack:
		ProcessAttackStateTransition();
		break;
	case enBossState_MagicAttack:
		ProcessMagicAttackStateTransition();
		break;
	case enBossState_ReceiveDamage:
		ProcessReceiveDamageStateTransition();
		break;
	case enBossState_Down:
		ProcessDownStateTransition();
		break;
	}
}

void Boss::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.5f);
	switch (m_BossState)
	{
	case enBossState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.5f);
		break;
		/*case enBossState_Walk:
			m_modelRender.SetAnimationSpeed(1.2f);
			m_modelRender.PlayAnimation(enAnimationClip_Walk, 0.1f);
			break;
		case enBossState_Run:
			m_modelRender.SetAnimationSpeed(1.2f);
			m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
			break;
		*/
	case enBossState_Chase:
		m_modelRender.SetAnimationSpeed(1.6f);
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	case enBossState_Attack:
		m_modelRender.SetAnimationSpeed(0.9f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.1f);
		break;
	case enBossState_MagicAttack:
		m_modelRender.SetAnimationSpeed(1.5f);
		m_modelRender.PlayAnimation(enAnimationClip_MagicAttack, 0.1f);
		break;
	case enBossState_ReceiveDamage:
		m_modelRender.SetAnimationSpeed(1.8f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.1f);
		break;
	case enBossState_Down:
		m_modelRender.PlayAnimation(enAnimationClip_Down, 0.1f);
		break;
	default:
		break;
	}
}

void Boss::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
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
		//rotation.AddRotationDegZ(180.0f);
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

const bool Boss::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	//距離がかなり近かったら。
	if (diff.LengthSq() <= 100.0f * 100.0f)
	{
		return true;
	}
	return false;
}

void Boss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}