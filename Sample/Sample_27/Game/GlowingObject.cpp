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
	//角度を設定。
	m_spotLight.SetAngle(Math::DegToRad(10.0f));
	//距離による、光の減衰率を設定。
	m_spotLight.SetRangeAffectPowParam(2.0f);
	

	Vector3 color;
	float a, b;
	a = m_position.x * m_position.z;
	b = m_position.x + m_position.z;
	color.x = int(fabsf(a)) % 2 + 1;
	color.y = int(fabsf(b)) % 2 + 1;
	color.z = int(fabsf(a * b)) % 2 + 1;
	//ライトの色を設定。
	m_spotLight.SetColor(color * 3.0f);
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