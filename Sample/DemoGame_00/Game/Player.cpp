#include "stdafx.h"
#include "Player.h"

#include "Game.h"

#include "sound/SoundSource.h"
#include "sound/SoundEngine.h"

#ifdef USE_VOLUME_SPOTLIGHT
	#define ENABLE_PLAYER_SPOTLIGHT
#endif
namespace
{
	float GAMEOVER_LIMITED_POSITION = -200.0f;
}

Player::Player()
{

}

Player::~Player()
{
	// ポイントライトの削除
	for (auto pt : m_pointLightList) {
		delete pt;
	}
}

bool Player::Start()
{
	//アニメーションクリップをロードする。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/jump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(false);
	m_animationClips[enAnimationClip_GameClear].Load("Assets/animData/clear.tka");
	m_animationClips[enAnimationClip_GameClear].SetLoopFlag(false);
	m_animationClips[enAnimationClip_GameOver].Load("Assets/animData/KneelDown.tka");
	m_animationClips[enAnimationClip_GameOver].SetLoopFlag(false);
	//ユニティちゃんのモデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisY);

	//キャラクターコントローラーを初期化。
	m_charaCon.Init(
		20.0f,			//半径。
		100.0f,			//高さ。
		m_position		//座標。
	);

	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/gameover.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/gameclear.wav");
	// ポイントライトの生成
	//m_pointLight = g_sceneLight->NewPointLight();
	// スポットライトを初期化。
#ifdef ENABLE_PLAYER_SPOTLIGHT

	m_spotLight.Init();
	m_spotLight.SetRange(400.0f);
	m_spotLight.SetColor(2.0f, 2.0f, 2.0f);
	m_spotLight.SetAngle(Math::DegToRad(20.0f));
	m_spotLight.SetRangeAffectPowParam(1.0f);
	m_spotLight.SetAngleAffectPowParam(1.0f);
	

	m_spotLight.SetColor2(10.0f, 10.0f, 10.0f);	
	m_spotLight.SetAngle2(Math::DegToRad(20.0f));
	m_spotLight.SetRangeAffectPowParam2(10.0f);
	m_spotLight.SetAngleAffectPowParam2(5.0f);
	
	m_spotLight.SetColor3(20.0f, 20.0f, 20.0f);
	m_spotLight.SetAngle3(Math::DegToRad(30.0f));
	m_spotLight.SetRangeAffectPowParam3(40.0f);
	m_spotLight.SetAngleAffectPowParam3(0.1f);

	// ボリュームスポットライトの初期化。
	m_volumeSpotLight.Init(m_spotLight);
#endif
	return true;
}

void Player::Update()
{
	for (auto pt : m_pointLightList) {
		pt->Update();
	}
#ifdef ENABLE_PLAYER_SPOTLIGHT
	m_spotLight.Update();
	Vector3 pos = m_position;
	pos.y += 50.0f;
	m_spotLight.SetPosition(pos);

	m_spotLight.SetDirection(m_forward);
	m_volumeSpotLight.Update();
#endif
	//移動処理。
	Move();
	//旋回処理。
	Turn();
	//アニメーション再生。
	PlayAnimation();
	//ステート管理。
	ManageState();

	//モデルを更新。
	m_modelRender.Update();

	auto mRot = Matrix::Identity;
	mRot.MakeRotationFromQuaternion(m_rotation);
	m_forward.x = mRot.m[2][0];
	m_forward.y = mRot.m[2][1];
	m_forward.z = mRot.m[2][2];
	
	// ポイントライト更新
	/*m_pointLight->SetPosition( m_position.x, m_position.y + 50.0f, m_position.z );
	m_pointLight->SetColor(7.0f, 7.0f, 7.0f);
	m_pointLight->SetRange(200.0f);
	m_pointLight->SetAffectPowParam( 0.5f );*/
	// ポイントライト大量設置のテスト
	if (g_pad[0]->IsTrigger(enButtonB)) {
		auto newPt = new PointLight;
		newPt->Init();
		// 現在のプレイヤーのポイントライトをコピー。
		newPt->SetPosition(m_position.x, m_position.y + 50.0f, m_position.z);
		newPt->SetColor(7.0f, 7.0f, 7.0f);
		newPt->SetRange(200.0f);
		newPt->SetAffectPowParam(0.5f);
		
		m_pointLightList.push_back(newPt);
	}
	
}

void Player::Move()
{
	
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	//ゲームオーバーかゲームクリアの際には、プレイヤーを移動させない。
	if(m_playerState != enPlayerState_GameClear &&
		m_playerState != enPlayerState_GameClear_Idle &&
		m_playerState != enPlayerState_GameOver)
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
		m_moveSpeed += cameraForward * lStick_y * 500.0f;	//奥方向への移動速度を加算。
		m_moveSpeed += cameraRight * lStick_x * 500.0f;		//右方向への移動速度を加算。
		if (g_pad[0]->IsTrigger(enButtonA) //Aボタンが押されたら
			&& m_charaCon.IsOnGround()  //かつ、地面に居たら
			) {
			//ジャンプする。
			m_moveSpeed.y = 400.0f;	//上方向に速度を設定して、
		}
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
}

void Player::Turn()
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

void Player::ManageState()
{
	if (m_playerState == enPlayerState_GameClear_Idle)
	{
		return;
	}

	if (m_playerState == enPlayerState_GameClear)
	{
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			//ステートをゲームクリア(待機)にする。
			m_playerState = enPlayerState_GameClear_Idle;
			Game* game = FindGO<Game>("game");
			game->NotifyGameClearIdle();
		}
		return;
	}

	//ゲームオーバーの時。
	if (m_playerState == enPlayerState_GameOver)
	{
		//アニメーションの再生が終わったら。
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			//ステートを待機状態にする。
			m_playerState = enPlayerState_Idle;
			Game* game = FindGO<Game>("game");
			game->NotifyReStart();
		}
		return;
	}

	//地面に付いていなかったら。
	if (m_charaCon.IsOnGround() == false)
	{
		//ステートをジャンプ中にする。
		m_playerState = enPlayerState_Jump;
		if (m_position.y <= GAMEOVER_LIMITED_POSITION)
		{
			Game* game = FindGO<Game>("game");
			game->NotifyGameOver();
		}
		//ここでManageStateの処理を終わらせる。
		return;
	}

	//地面に付いていたら。
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//ステートを走りにする。
		m_playerState = enPlayerState_Run;
	}
	//xとzの移動速度が無かったら(スティックの入力が無かったら)。
	else
	{
		//ステートを待機にする。
		m_playerState = enPlayerState_Idle;
	}
}

void Player::PlayAnimation()
{
	//switch文。
	switch (m_playerState) {
		//プレイヤーステートが0(待機)だったら。
	case enPlayerState_Idle:
		//待機アニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//プレイヤーステートがジャンプ中だったら。
	case enPlayerState_Jump:
		//ジャンプアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Jump);
		break;
		//プレイヤーステートが走りだったら。
	case enPlayerState_Run:
		//歩きアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
		//プレイヤーステートがゲームクリアだったら。
	case enPlayerState_GameClear:
		//ゲームクリアアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_GameClear);
		break;
		//プレイヤーステートがゲームオーバーだったら。
	case enPlayerState_GameOver:
		//ゲームオーバーアニメーションを再生する。
		m_modelRender.PlayAnimation(enAnimationClip_GameOver);
		break;
	}
}

void Player::NotifyGameOver()
{
	m_playerState = enPlayerState_GameOver;
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(3);
	se->SetVolume(3.f);
	se->Play(false);
}

void Player::NotifyGameClear()
{
	m_playerState = enPlayerState_GameClear;
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(4);
	se->SetVolume(3.f);
	se->Play(false);
}

void Player::ReStart()
{
	m_position = m_startPosition;
	m_moveSpeed = Vector3::Zero;
	m_modelRender.SetPosition(m_position);
	m_charaCon.SetPosition(m_position);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}