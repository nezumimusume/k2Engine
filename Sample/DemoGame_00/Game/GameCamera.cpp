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
	//�����_���王�_�܂ł̃x�N�g����ݒ�B
	m_toCameraPos.Set(0.0f, 200.0f, 400.0f);

	//�v���C���[�̃C���X�^���X��T���B
	m_player = FindGO<Player>("player");

	//�΂˃J�����̏������B
	m_springCamera.Init(
		*g_camera3D,		//�΂˃J�����̏������s���J�������w�肷��B
		1000.0f,			//�J�����̈ړ����x�̍ő�l�B
		true,				//�J�����ƒn�`�Ƃ̂����蔻�����邩�ǂ����̃t���O�Btrue���Ƃ����蔻����s���B
		5.0f				//�J�����ɐݒ肳��鋅�̃R���W�����̔��a�B��R������true�̎��ɗL���ɂȂ�B
	);
	return true;
}

void GameCamera::UpdatePositionAndTarget()
{
	//�v���C���[���Q�[���I�[�o�[�܂��̓Q�[���N���A��Ԃ̎��B
	//�J�����𑀍�ł��Ȃ��悤�ɂ���B
	if (m_player->GetPlayerState() == Player::enPlayerState_GameClear ||
		m_player->GetPlayerState() == Player::enPlayerState_GameClear_Idle ||
		m_player->GetPlayerState() == Player::enPlayerState_GameOver)
	{
		return;
	}

	//�J�������X�V�B
	//�����_���v�Z����B
	Vector3 target = m_player->GetPosition();
	//�v���C���̑������炿����Ə�𒍎��_�Ƃ���B
	target.y += 80.0f;
	target += g_camera3D->GetForward() * 20.0f;

	Vector3 toCameraPosOld = m_toCameraPos;
	//�p�b�h�̓��͂��g���ăJ�������񂷁B
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y������̉�]
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 2.0f * x);
	qRot.Apply(m_toCameraPos);
	//X������̉�]�B
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 2.0f * y);
	qRot.Apply(m_toCameraPos);
	//�J�����̉�]�̏�����`�F�b�N����B
	//�����_���王�_�܂ł̃x�N�g���𐳋K������B
	//���K������ƁA�x�N�g���̑傫�����P�ɂȂ�B
	//�傫�����P�ɂȂ�Ƃ������Ƃ́A�x�N�g�����狭�����Ȃ��Ȃ�A�����݂̂̏��ƂȂ�Ƃ������ƁB
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//�J����������������B
		m_toCameraPos = toCameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//�J�����������������B
		m_toCameraPos = toCameraPosOld;
	}


	//���_���v�Z����B
	Vector3 pos = target + m_toCameraPos;

	//�o�l�J�����ɒ����_�Ǝ��_��ݒ肷��B
	m_springCamera.SetPosition(pos);
	m_springCamera.SetTarget(target);
}

void GameCamera::ReStart()
{
	//�����_���王�_�܂ł̃x�N�g����ݒ�B
	m_toCameraPos.Set(0.0f, 200.0f, 400.0f);
	m_springCamera.Refresh();
	UpdatePositionAndTarget();
}

void GameCamera::NotifyGameClear()
{
	Vector3 target = m_player->GetPosition();
	target.y += 50.0f;
	Vector3 pos = target + m_player->GetForward() * 200.0f;
	pos.y += 100.0f;
	m_springCamera.SetTarget(target);
	m_springCamera.SetPosition(pos);
}

void GameCamera::Update()
{
	//���_�ƒ����_���X�V����B
	UpdatePositionAndTarget();

	//�J�����̍X�V�B
	m_springCamera.Update();
}