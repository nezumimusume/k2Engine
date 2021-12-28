#include "stdafx.h"
#include "GlowingObject.h"

GlowingObject::GlowingObject()
{

}

GlowingObject::~GlowingObject()
{

}

bool GlowingObject::Start()
{
	m_modelRender.Init("Assets/modelData/light.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	//�X�|�b�g���C�g���������B
	m_spotLight.Init();
	//���C�g�̗L��������ݒ�B
	m_spotLight.SetRange(600.0f);
	//�p�x��ݒ�B
	m_spotLight.SetAngle(Math::DegToRad(10.0f));
	//�����ɂ��A���̌�������ݒ�B
	m_spotLight.SetRangeAffectPowParam(2.0f);
	

	Vector3 color;
	float a, b;
	a = m_position.x * m_position.z;
	b = m_position.x + m_position.z;
	color.x = int(fabsf(a)) % 2 + 1;
	color.y = int(fabsf(b)) % 2 + 1;
	color.z = int(fabsf(a * b)) % 2 + 1;
	//���C�g�̐F��ݒ�B
	m_spotLight.SetColor(color * 3.0f);
	Vector3 direction = Vector3::AxisY;
	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(direction);
	//���C�g�̍��W��ݒ�B
	m_spotLight.SetPosition(m_position);
	//�{�����[�����C�g���X�|�b�g���C�g�ŏ������B
	m_volumeSpotLight.Init(m_spotLight);
	return true;
}

void GlowingObject::Update()
{
	m_timer += g_gameTime->GetFrameDeltaTime();
	float angle = cosf(m_timer) * 1.7f;
	m_rotation.AddRotationDegZ(-angle);
	Vector3 direction = Vector3::AxisY;
	m_rotation.Apply(direction);

	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(direction);
	//�X�|�b�g���C�g���X�V�B
	m_spotLight.Update();
	//�{�����[�����C�g���X�V�B
	m_volumeSpotLight.Update();

	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
}

void GlowingObject::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}