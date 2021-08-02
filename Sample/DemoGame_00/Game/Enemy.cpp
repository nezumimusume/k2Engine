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
	//�A�j���[�V�����N���b�v�����[�h����B
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/enemy/idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/enemy/run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	//�G�l�~�[�̃��f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/enemy.tkm", m_animationClips, enAnimationClip_Num, enModelUpAxisZ);
	//���W��傫����ݒ�B
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetScale(m_scale);

	m_movedPath->Init(m_position, MOVE_SPPED);
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
	return;
	if (GetPlayer() == false)
	{
		return;
	}

	//�ړ������B
	Move();
	//���񏈗��B
	Turn();
	//�X�e�[�g�Ǘ��B
	ManageState();
	//�A�j���[�V�����Đ��B
	PlayAnimation();
	//�v���C���[��T���B
	SearchPlayer();

	//���f�����X�V�B
	m_modelRender.Update();
}

void Enemy::Move()
{
	//�v���C���[���Q�[���I�[�o�[���Q�[���N���A�Ȃ�ړ������Ȃ��B
	if (m_player->GetPlayerState() == Player::enPlayerState_GameClear ||
		m_player->GetPlayerState() == Player::enPlayerState_GameOver ||
		m_player->GetPlayerState() == Player::enPlayerState_GameClear_Idle)
	{
		return;
	}

	m_position = m_movedPath->Move();
	m_moveVector = m_movedPath->GetMoveVector();
	//���W��ݒ�B
	m_modelRender.SetPosition(m_position);
}

void Enemy::Turn()
{
	//atan2��tan�Ƃ̒l���p�x(���W�A���P��)�ɕϊ����Ă����֐��B
	//m_moveVector.x / m_moveVector.z�̌��ʂ�tan�ƂɂȂ�B
	//atan2���g�p���āA�p�x�����߂Ă���B
	//���ꂪ��]�p�x�ɂȂ�B
	float angle = atan2(-m_moveVector.x, m_moveVector.z);
	//atan���Ԃ��Ă���p�x�̓��W�A���P�ʂȂ̂�
	//SetRotationDeg�ł͂Ȃ�SetRotation���g�p����B
	m_rotation.SetRotationY(-angle);

	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);
}

void Enemy::ManageState()
{
	//��~���Ȃ�B
	if (m_movedPath->GetIsStop())
	{
		//�X�e�[�g��ҋ@���ɂ���B
		m_enemyState = enEnemyState_Idle;
	}
	//�ړ����Ȃ�B
	else
	{
		//�X�e�[�g������ɂ���B
		m_enemyState = enEnemyState_Run;
	}
}

void Enemy::PlayAnimation()
{
	//switch���B
	switch (m_enemyState) {
		//�v���C���[�X�e�[�g���ҋ@��������B
	case enEnemyState_Idle:
		//�ҋ@�A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//�v���C���[�X�e�[�g�����肾������B
	case enEnemyState_Run:
		//����A�j���[�V�������Đ�����B
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
	}
}

void Enemy::SearchPlayer()
{
	//�v���C���[���Q�[���I�[�o�[���Q�[���N���A�Ȃ�ړ������Ȃ��B
	if (m_player->GetPlayerState() == Player::enPlayerState_GameClear ||
		m_player->GetPlayerState() == Player::enPlayerState_GameOver ||
		m_player->GetPlayerState() == Player::enPlayerState_GameClear_Idle)
	{
		return;
	}

	//�G����v���C���[�Ɍ������x�N�g�����v�Z�B
	Vector3 toPlayer = m_player->GetPosition() - m_position;
	if (toPlayer.Length() < SERACH_PLAYER_DISTANCE) {
		//����p�𔻒�B
		toPlayer.Normalize();

		float angle = acosf(toPlayer.Dot(m_moveVector));
		if (fabsf(angle) < Math::PI * SERACH_PLAYER_ANGLE) {
			//����p45�x�ȓ��B
			//�Q�[���ɃQ�[���I�[�o�[��ʒm�B
			Game* game = FindGO<Game>("game");
			game->NotifyGameOver();
		}
	}
}

void Enemy::Render(RenderContext& rc)
{
	return;
	m_modelRender.Draw(rc);
}