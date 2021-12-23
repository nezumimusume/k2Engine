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
	//�L�����N�^�[�R���g���[���[���������B
	m_charaCon.Init(
		20.0f,			//���a�B
		100.0f,			//�����B
		m_position		//���W�B
	);

	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	m_spotLight.Init();

	//�X�|�b�g���C�g���������B
	m_spotLight.Init();
	//���C�g�̗L��������ݒ�B
	m_spotLight.SetRange(300.0f);
	//�p�x��ݒ�B
	m_spotLight.SetAngle(Math::DegToRad(20.0f));
	//�����ɂ��A���̌�������ݒ�B
	m_spotLight.SetRangeAffectPowParam(1.5f);
	//�p�x�ɂ��A���̌�������ݒ�B
	m_spotLight.SetAngleAffectPowParam(0.7f);
	//���C�g�̐F��ݒ�B
	m_spotLight.SetColor(Vector3::One);
	//���C�g�̍��W��ݒ�B
	m_spotLight.SetPosition(m_position + Vector3(0.0f, 30.0f, 0.0f));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(m_forward);

	//�{�����[�����C�g���X�|�b�g���C�g�ŏ������B
	m_volumeSpotLight.Init(m_spotLight);
	return true;
}

void Player::Update()
{
	//�ړ������B
	Move();
	//��]�����B
	Rotation();

	//���f�����X�V�B
	m_modelRender.Update();

	//�X�|�b�g���C�g���X�V�B
	m_spotLight.Update();
	//�{�����[�����C�g���X�V�B
	m_volumeSpotLight.Update();
}

void Player::Move()
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
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	m_moveSpeed += cameraForward * lStick_y * 200.0f;	//�������ւ̈ړ����x�����Z�B
	m_moveSpeed += cameraRight * lStick_x * 200.0f;		//�E�����ւ̈ړ����x�����Z�B
	if (g_pad[0]->IsTrigger(enButtonA) //A�{�^���������ꂽ��
		&& m_charaCon.IsOnGround()  //���A�n�ʂɋ�����
		) {
		//�W�����v����B
		m_moveSpeed.y = 400.0f;	//������ɑ��x��ݒ肵�āA
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

	//���C�g�̍��W��ݒ�B
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

		//���C�g�̕�����ݒ�B
		m_spotLight.SetDirection(m_forward);
	}

}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}