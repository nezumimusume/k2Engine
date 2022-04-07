#include "stdafx.h"
#include "BackGround.h"

Background::Background()
{

}

Background::~Background()
{

}

bool Background::Start()
{
	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/background.tkm");
	//Y����180����]�B
	Quaternion rot;
	rot.SetRotationDegY(180.0f);
	//��]��ݒ�B
	m_modelRender.SetRotation(rot);
	//�傫����ݒ�B
	m_modelRender.SetScale({ 0.5f,0.5f,0.5f });
	//�傫���A��]�̕ύX�𔽉f�����邽�߂ɁA���f�����X�V�B
	m_modelRender.Update();

	//���C���������ăS�[�X�g�������ɂ����̂ō폜�B
	//m_phyStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetWorldMatrix(0));

	return true;
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}