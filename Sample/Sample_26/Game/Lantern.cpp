#include "stdafx.h"
#include "Lantern.h"

Lantern::Lantern()
{

}

Lantern::~Lantern()
{

}

bool Lantern::Start()
{
	m_modelRender.Init("Assets/modelData/Lantern.tkm");

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);
	// �X�|�b�g���C�g���������B
	m_spotLight.Init();
	//���C�g�̉e��������ݒ�B
	m_spotLight.SetRange(600.0f);
	//���C�g�̉e���p�x��ݒ�B
	m_spotLight.SetAngle(Math::DegToRad(40.0f));
	//���C�g�̋�����������ݒ�B
	m_spotLight.SetRangeAffectPowParam(1.0f);
	//���C�g�̊p�x��������ݒ�B
	m_spotLight.SetAngleAffectPowParam(0.7f);
	//���C�g�̐F��ݒ�B
	m_spotLight.SetColor(30.0f, 0.0f, 0.0f);
	//���C�g�̕�����ݒ�B
	m_spotLight.SetDirection(Vector3(0.0f,-1.0f,0.0f));
	//���C�g�̍��W��ݒ�B
	m_spotLight.SetPosition(m_position + Vector3(0.0f,250.0f,0.0f));
	return true;
}

void Lantern::Update()
{
	m_modelRender.Update();
	m_spotLight.Update();
}

void Lantern::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}