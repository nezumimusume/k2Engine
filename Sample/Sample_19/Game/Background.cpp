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
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/background.tkm");
	//Y軸に180°回転。
	Quaternion rot;
	rot.SetRotationDegY(180.0f);
	//回転を設定。
	m_modelRender.SetRotation(rot);
	//大きさを設定。
	m_modelRender.SetScale({ 0.5f,0.5f,0.5f });
	//大きさ、回転の変更を反映させるために、モデルを更新。
	m_modelRender.Update();

	//ラインが多くてゴーストが見えにくいので削除。
	//m_phyStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetWorldMatrix(0));

	return true;
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}