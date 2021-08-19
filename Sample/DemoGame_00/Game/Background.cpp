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
	//��]��ݒ�B
	m_modelRender.SetRotation(m_rotation);
	//�傫����ݒ�B
	m_modelRender.SetScale(m_scale);
	//��]��ݒ�B
	m_modelRender.SetRotation(m_rotation);
	//�傫���A��]�̕ύX�𔽉f�����邽�߂ɁA���f�����X�V�B
	m_modelRender.Update();

	//�ÓI�����I�u�W�F�N�g���쐬�B
	m_phyStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	return true;
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}