#include "k2EnginePreCompile.h"
#include "SceneLight.h"

void PointLight::Update()
{
    // 使用中のライトはカメラ空間での座標を計算する。
    if (!isUse) {
        return;
    }
    positionInView = position;
    g_camera3D->GetViewMatrix().Apply(positionInView);
}
void SceneLight::Init()
{
    // 太陽光
    m_light.directionalLight[0].color.x = 2.0f;
    m_light.directionalLight[0].color.y = 2.0f;
    m_light.directionalLight[0].color.z = 2.0f;

    m_light.directionalLight[0].direction.x = 1.0f;
    m_light.directionalLight[0].direction.y = -1.0f;
    m_light.directionalLight[0].direction.z = -1.0f;
    m_light.directionalLight[0].direction.Normalize();
    m_light.directionalLight[0].castShadow = true;

    //
    m_light.directionalLight[1].color.x = 1.0f;
    m_light.directionalLight[1].color.y = 1.0f;
    m_light.directionalLight[1].color.z = 1.0f;

    m_light.directionalLight[1].direction.x = -1.0f;
    m_light.directionalLight[1].direction.y = -1.0f;
    m_light.directionalLight[1].direction.z = 1.0f;
    m_light.directionalLight[1].direction.Normalize();
    m_light.directionalLight[1].castShadow = false;

    // 地面からの照り返し
    m_light.directionalLight[2].color.x = 0.6f;
    m_light.directionalLight[2].color.y = 0.6f;
    m_light.directionalLight[2].color.z = 0.6f;

    m_light.directionalLight[2].direction.x = -1.0f;
    m_light.directionalLight[2].direction.y = 1.0f;
    m_light.directionalLight[2].direction.z = 1.0f;
    m_light.directionalLight[2].direction.Normalize();

    m_light.ambinetLight.x = 0.4f;
    m_light.ambinetLight.y = 0.4f;
    m_light.ambinetLight.z = 0.4f;
    m_light.eyePos = g_camera3D->GetPosition();
    m_light.numPointLight = 0;

    // 全てのポイントライトを未使用にする
    for (auto& pt : m_light.pointLights) {
        pt.UnUse();
        pt.SetAffectPowParam(1.0f);
        m_unusePointLightQueue.push_back(&pt);
    }
}
void SceneLight::Update()
{
    for (auto& pt : m_light.pointLights) {
        pt.Update();
    }
    m_light.numPointLight = MAX_POINT_LIGHT - static_cast<int>(m_unusePointLightQueue.size());
}