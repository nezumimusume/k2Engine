#include "stdafx.h"
#include "Player.h"

#include "Game.h"

#include "sound/SoundSource.h"
#include "sound/SoundEngine.h"

namespace
{
	float GAMEOVER_LIMITED_POSITION = -200.0f;
}

Player::Player()
{

}

Player::~Player()
{
	// �|�C���g���C�g�̍폜
	for (auto pt : m_pointLightList) {
		delete pt;
	}
}

bool Player::Start()
{
	//�A�j���[�V�����N���b�v�����[�h����B
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
	//���j�e�B�����̃��f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/unityChan.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisY);

	//�L�����N�^�[�R���g���[���[���������B
	m_charaCon.Init(
		20.0f,			//���a�B
		100.0f,			//�����B
		m_position		//���W�B
	);

	g_soundEngine->ResistWaveFileBank(3, "Assets/sound/gameover.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/gameclear.wav");
	// �|�C���g���C�g�̐���
	//m_pointLight = g_sceneLight->NewPointLight();
	// �X�|�b�g���C�g���������B
	m_spotLight.Init();
	m_spotLight.SetRange(800.0f);
	m_spotLight.SetAngle(Math::DegToRad(20.0f));
	m_spotLight.SetRangeAffectPowParam(5.0f);
	m_spotLight.SetAngleAffectPowParam(1.0f);
	m_spotLight.SetColor(1.0f, 1.0f, 1.0f);

	// �{�����[���X�|�b�g���C�g�̏������B
	m_volumeSpotLight.Init(m_spotLight);
	return true;
}

void Player::Update()
{
	for (auto pt : m_pointLightList) {
		pt->Update();
	}
	Vector3 color = m_spotLight.GetColor();
	float affect_0 = m_spotLight.GetAngleAffectPowParam();
	float affect_1 = m_spotLight.GetRangeAffectPowParam();
	float angle = m_spotLight.GetAngle();
	float range = m_spotLight.GetRange();
	if( GetAsyncKeyState('Z'))
	{
		color.x += 0.1f;
	}
	if (GetAsyncKeyState('X'))
	{
		color.y += 0.1f;
	}
	if (GetAsyncKeyState('C'))
	{
		color.z += 0.1f;
	}
	if (GetAsyncKeyState('V')) {
		range += 10.0f;
	}
	if (GetAsyncKeyState('B'))
	{
		affect_0 -= 0.01f;
	}
	if (GetAsyncKeyState('N')) {
		angle -= 0.01f;
	}
	if (GetAsyncKeyState('M')) {
		angle += 0.01f;
	}
	m_spotLight.SetRange(range);
	m_spotLight.SetRangeAffectPowParam(affect_1);
	m_spotLight.SetAngleAffectPowParam(affect_0);
	m_spotLight.SetAngle(angle);
	m_spotLight.SetColor(color);
	m_spotLight.Update();
	//�ړ������B
	Move();
	//���񏈗��B
	Turn();
	//�A�j���[�V�����Đ��B
	PlayAnimation();
	//�X�e�[�g�Ǘ��B
	ManageState();

	//���f�����X�V�B
	m_modelRender.Update();

	auto mRot = Matrix::Identity;
	mRot.MakeRotationFromQuaternion(m_rotation);
	m_forward.x = mRot.m[2][0];
	m_forward.y = mRot.m[2][1];
	m_forward.z = mRot.m[2][2];
	
	// �|�C���g���C�g�X�V
	/*m_pointLight->SetPosition( m_position.x, m_position.y + 50.0f, m_position.z );
	m_pointLight->SetColor(7.0f, 7.0f, 7.0f);
	m_pointLight->SetRange(200.0f);
	m_pointLight->SetAffectPowParam( 0.5f );*/
	// �|�C���g���C�g��ʐݒu�̃e�X�g
	if (g_pad[0]->IsTrigger(enButtonB)) {
		auto newPt = new PointLight;
		newPt->Init();
		// ���݂̃v���C���[�̃|�C���g���C�g���R�s�[�B
		newPt->SetPosition(m_position.x, m_position.y + 50.0f, m_position.z);
		newPt->SetColor(7.0f, 7.0f, 7.0f);
		newPt->SetRange(200.0f);
		newPt->SetAffectPowParam(0.5f);
		
		m_pointLightList.push_back(newPt);
	}
	Vector3 pos = m_position;
	pos.y += 50.0f;
	m_spotLight.SetPosition(pos);

	m_spotLight.SetDirection(m_forward);
	m_volumeSpotLight.Update();
}

void Player::Move()
{
	
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	//�Q�[���I�[�o�[���Q�[���N���A�̍ۂɂ́A�v���C���[���ړ������Ȃ��B
	if(m_playerState != enPlayerState_GameClear &&
		m_playerState != enPlayerState_GameClear_Idle &&
		m_playerState != enPlayerState_GameOver)
	{
		//���̃t���[���̈ړ��ʂ����߂�B
		//���X�e�B�b�N�̓��͗ʂ��󂯎��B
		float lStick_x = g_pad[0]->GetLStickXF();
		float lStick_y = g_pad[0]->GetLStickYF();
		//�J�����̑O�������ƉE�������擾�B
		Vector3 cameraForward = g_camera3D->GetForward();
		Vector3 cameraRight = g_camera3D->GetRight();
		//XZ���ʂł̑O�������A�E�����ɕϊ�����B
		cameraForward.y = 0.0f;
		cameraForward.Normalize();
		cameraRight.y = 0.0f;
		cameraRight.Normalize();
		//XZ�����̈ړ����x���N���A�B
		m_moveSpeed += cameraForward * lStick_y * 500.0f;	//�������ւ̈ړ����x�����Z�B
		m_moveSpeed += cameraRight * lStick_x * 500.0f;		//�E�����ւ̈ړ����x�����Z�B
		if (g_pad[0]->IsTrigger(enButtonA) //A�{�^���������ꂽ��
			&& m_charaCon.IsOnGround()  //���A�n�ʂɋ�����
			) {
			//�W�����v����B
			m_moveSpeed.y = 400.0f;	//������ɑ��x��ݒ肵�āA
		}
	}

	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();
	//�L�����N�^�[�R���g���[���[���g�p���āA���W���X�V�B

	m_position = m_charaCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
	if (m_charaCon.IsOnGround()) {
		//�n�ʂɂ����B
		m_moveSpeed.y = 0.0f;
	}

	//���W��ݒ�B
	m_modelRender.SetPosition(m_position);
}

void Player::Turn()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		//m_moveSpeed.x��m_moveSpeed.z�̐�Βl���Ƃ���0.001�ȉ��Ƃ������Ƃ�
		//���̃t���[���ł̓L�����͈ړ����Ă��Ȃ��̂Ő��񂷂�K�v�͂Ȃ��B
		return;
	}
	//atan2��tan�Ƃ̒l���p�x(���W�A���P��)�ɕϊ����Ă����֐��B
	//m_moveSpeed.x / m_moveSpeed.z�̌��ʂ�tan�ƂɂȂ�B
	//atan2���g�p���āA�p�x�����߂Ă���B
	//���ꂪ��]�p�x�ɂȂ�B
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	//atan���Ԃ��Ă���p�x�̓��W�A���P�ʂȂ̂�
	//SetRotationDeg�ł͂Ȃ�SetRotation���g�p����B
	m_rotation.SetRotationY(-angle);

	//��]��ݒ肷��B
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
			//�X�e�[�g���Q�[���N���A(�ҋ@)�ɂ���B
			m_playerState = enPlayerState_GameClear_Idle;
			Game* game = FindGO<Game>("game");
			game->NotifyGameClearIdle();
		}
		return;
	}

	//�Q�[���I�[�o�[�̎��B
	if (m_playerState == enPlayerState_GameOver)
	{
		//�A�j���[�V�����̍Đ����I�������B
		if (m_modelRender.IsPlayingAnimation() == false)
		{
			//�X�e�[�g��ҋ@��Ԃɂ���B
			m_playerState = enPlayerState_Idle;
			Game* game = FindGO<Game>("game");
			game->NotifyReStart();
		}
		return;
	}

	//�n�ʂɕt���Ă��Ȃ�������B
	if (m_charaCon.IsOnGround() == false)
	{
		//�X�e�[�g���W�����v���ɂ���B
		m_playerState = enPlayerState_Jump;
		if (m_position.y <= GAMEOVER_LIMITED_POSITION)
		{
			Game* game = FindGO<Game>("game");
			game->NotifyGameOver();
		}
		//������ManageState�̏������I��点��B
		return;
	}

	//�n�ʂɕt���Ă�����B
	//x��z�̈ړ����x����������(�X�e�B�b�N�̓��͂���������)�B
	if (fabsf(m_moveSpeed.x) >= 0.001f || fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//�X�e�[�g�𑖂�ɂ���B
		m_playerState = enPlayerState_Run;
	}
	//x��z�̈ړ����x������������(�X�e�B�b�N�̓��͂�����������)�B
	else
	{
		//�X�e�[�g��ҋ@�ɂ���B
		m_playerState = enPlayerState_Idle;
	}
}

void Player::PlayAnimation()
{
	//switch���B
	switch (m_playerState) {
		//�v���C���[�X�e�[�g��0(�ҋ@)��������B
	case enPlayerState_Idle:
		//�ҋ@�A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//�v���C���[�X�e�[�g���W�����v����������B
	case enPlayerState_Jump:
		//�W�����v�A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_Jump);
		break;
		//�v���C���[�X�e�[�g�����肾������B
	case enPlayerState_Run:
		//�����A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
		//�v���C���[�X�e�[�g���Q�[���N���A��������B
	case enPlayerState_GameClear:
		//�Q�[���N���A�A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_GameClear);
		break;
		//�v���C���[�X�e�[�g���Q�[���I�[�o�[��������B
	case enPlayerState_GameOver:
		//�Q�[���I�[�o�[�A�j���[�V�������Đ�����B
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