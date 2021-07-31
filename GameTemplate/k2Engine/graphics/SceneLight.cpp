#include "k2EnginePreCompile.h"
#include "SceneLight.h"

void SceneLight::Init()
{


    // ‘¾—zŒõ
    m_light.directionalLight[0].color.x = 3.0f;
    m_light.directionalLight[0].color.y = 3.0f;
    m_light.directionalLight[0].color.z = 3.0f;

    m_light.directionalLight[0].direction.x = 1.0f;
    m_light.directionalLight[0].direction.y = -1.0f;
    m_light.directionalLight[0].direction.z = 1.0f;
    m_light.directionalLight[0].direction.Normalize();
    m_light.directionalLight[0].castShadow = true;

    //
    m_light.directionalLight[1].color.x = 3.0f;
    m_light.directionalLight[1].color.y = 3.0f;
    m_light.directionalLight[1].color.z = 3.0f;

    m_light.directionalLight[1].direction.x = -1.0f;
    m_light.directionalLight[1].direction.y = -1.0f;
    m_light.directionalLight[1].direction.z = 1.0f;
    m_light.directionalLight[1].direction.Normalize();
    m_light.directionalLight[1].castShadow = true;

    // ’n–Ê‚©‚ç‚ÌÆ‚è•Ô‚µ
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
}