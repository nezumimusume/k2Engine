#include "stdafx.h"
#include "Background.h"

Background::Background()
{

}

Background::~Background()
{

}

bool Background::Start()
{
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/background/background.tkm");
	//座標を設定する。
	m_modelRender.SetPosition(m_position);
	//大きさを設定する。
	m_modelRender.SetScale(m_scale);
	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
	//モデルを更新する。
	m_modelRender.Update();

	//当たり判定を作成する。
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	return true;
}

void Background::Update()
{
	//モデルの更新処理。
	m_modelRender.Update();
}

void Background::Render(RenderContext& rc)
{
	//モデルを描画する。
	m_modelRender.Draw(rc);
}