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

	//スポットライトを初期化。
	m_spotLight.Init();
	//ライトの有効距離を設定。
	m_spotLight.SetRange(600.0f);

	Vector3 color;
	float a, b;
	a = m_position.x * m_position.z;
	b = m_position.x + m_position.z;
	color.x = int(fabsf(a)) % 2 + 1;
	color.y = int(fabsf(b)) % 2 + 1;
	color.z = int(fabsf(a * b)) % 2 + 1;

	// １本目は 大外まで広がる光。
	m_spotLight.SetColor(color);
	m_spotLight.SetAngle(Math::DegToRad(15.0f));
	m_spotLight.SetRangeAffectPowParam(2.0f);
	m_spotLight.SetAngleAffectPowParam(2.0f);
	
	// ２本目は根元の光
	m_spotLight.SetColor2({ 1000.0f, 1000.0f, 1000.0f });
	m_spotLight.SetAngle2(Math::DegToRad(15.0f));
	m_spotLight.SetRangeAffectPowParam2(40.0f);
	m_spotLight.SetAngleAffectPowParam2(0.5f);

	// 3本目は中心を通っている光。
	m_spotLight.SetColor3(color * 2.2f);
	m_spotLight.SetAngle3(Math::DegToRad(15.0f));
	m_spotLight.SetRangeAffectPowParam3(3.0f);
	m_spotLight.SetAngleAffectPowParam3(2.0f);	

	
	Vector3 direction = Vector3::AxisY;
	//ライトの方向を設定。
	m_spotLight.SetDirection(direction);
	//ライトの座標を設定。
	m_spotLight.SetPosition(m_position);
	//ボリュームライトをスポットライトで初期化。
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

	//ライトの方向を設定。
	m_spotLight.SetDirection(direction);
	//スポットライトを更新。
	m_spotLight.Update();
	//ボリュームライトを更新。
	m_volumeSpotLight.Update();

	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
}

void GlowingObject::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}