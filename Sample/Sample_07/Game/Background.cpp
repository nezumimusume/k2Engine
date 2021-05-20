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
	m_modelRender.Init("Assets/modelData/stage/stage.tkm");
	//大きさ変更。
	m_modelRender.SetScale({ 0.7f, 0.7f, 0.7f });
	//大きさの変更を反映させるために、モデルを更新。
	m_modelRender.Update();

	//静的物理オブジェクトを作成。
	m_phyStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	//当たり判定描画を有効。
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	return true;
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}