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
	// スポットライトを初期化。
	m_spotLight.Init();
	//ライトの影響距離を設定。
	m_spotLight.SetRange(600.0f);
	//ライトの影響角度を設定。
	m_spotLight.SetAngle(Math::DegToRad(40.0f));
	//ライトの距離減衰率を設定。
	m_spotLight.SetRangeAffectPowParam(1.0f);
	//ライトの角度減衰率を設定。
	m_spotLight.SetAngleAffectPowParam(0.7f);
	//ライトの色を設定。
	m_spotLight.SetColor(30.0f, 0.0f, 0.0f);
	//ライトの方向を設定。
	m_spotLight.SetDirection(Vector3(0.0f,-1.0f,0.0f));
	//ライトの座標を設定。
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