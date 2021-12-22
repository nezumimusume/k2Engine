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
	// モデルを読み込む。
	m_modelRender.Init("Assets/modelData/background.tkm");
	// 回転を設定。
	m_modelRender.SetRotation(m_rotation);
	// 大きさを設定。
	m_modelRender.SetScale(m_scale);
	// 回転を設定。
	m_modelRender.SetRotation(m_rotation);
	// 大きさ、回転の変更を反映させるために、モデルを更新。
	m_modelRender.Update();
	// 静的物理オブジェクトを作成。
	m_phyStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	return true;
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}