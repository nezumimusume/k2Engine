#include "k2EnginePreCompile.h"
#include "SceneLight.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
    void PointLight::Update()
    {
        // 使用中のライトはカメラ空間での座標を計算する。
        if (!isUse) {
            return;
        }
        positionInView = position;
        g_camera3D->GetViewMatrix().Apply(positionInView);
    }
    void SpotLight::Update()
    {
        // 使用中のライトはカメラ空間での座標を計算する。
        if (!isUse) {
            return;
        }
        positionInView = position;
        g_camera3D->GetViewMatrix().Apply(positionInView);
        Matrix cameraRotInv = g_camera3D->GetCameraRotation();
        cameraRotInv.Inverse();
        cameraRotInv.Apply(directionInView);
    }
    void SceneLight::Init()
    {
        // 太陽光
        m_light.directionalLight[0].color.x = 1.2f;
        m_light.directionalLight[0].color.y = 1.2f;
        m_light.directionalLight[0].color.z = 1.2f;

        m_light.directionalLight[0].direction.x = 1.0f;
        m_light.directionalLight[0].direction.y = -1.0f;
        m_light.directionalLight[0].direction.z = -1.0f;
        m_light.directionalLight[0].direction.Normalize();
        m_light.directionalLight[0].castShadow = true;

        //
      /*  m_light.directionalLight[1].color.x = 1.0f;
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
        m_light.directionalLight[2].direction.Normalize();*/

        m_light.ambinetLight.x = 0.1f;
        m_light.ambinetLight.y = 0.1f;
        m_light.ambinetLight.z = 0.1f;
        m_light.eyePos = g_camera3D->GetPosition();
        m_light.numPointLight = 0;

        // 全てのポイントライトを未使用にする
        for (auto& pt : m_light.pointLights) {
            pt.UnUse();
            pt.SetAffectPowParam(1.0f);
            m_unusePointLightQueue.push_back(&pt);
        }
        // すべてのスポットライトを未使用にする。
        for (auto& sp : m_light.spotLights) {
            sp.UnUse();
            m_unuseSpotLightQueue.push_back(&sp);
        }
        // ボリュームライトマップを作成。
        m_volumeLightMap.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_mainRenderTargetFormat.colorBufferFormat,
            DXGI_FORMAT_UNKNOWN
        );
        m_volumeLightMapForward.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_mainRenderTargetFormat.colorBufferFormat,
            DXGI_FORMAT_UNKNOWN
        );
        m_volumeLightMapBack.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_mainRenderTargetFormat.colorBufferFormat,
            DXGI_FORMAT_UNKNOWN
        );

    }
    void SceneLight::SetIBLTextureForAmbient(const wchar_t* textureFilePath, float luminance)
    {
        g_renderingEngine->ReInitIBL(textureFilePath, luminance);
    }
    void SceneLight::DrawToVulumeLightMap(RenderContext& rc)
    {
        // 奥側の描画
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapBack);
        rc.SetRenderTargetAndViewport(m_volumeLightMapBack);
        rc.ClearRenderTargetView(m_volumeLightMapBack);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMap(rc);
        }
        // 続いて手前側
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapForward);
        rc.SetRenderTargetAndViewport(m_volumeLightMapForward);
        rc.ClearRenderTargetView(m_volumeLightMapForward);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMap(rc);
        }

        // 奥と手前の書き込み完了待ち
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapBack);
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapForward);

        // todo 続きはここから奥側と手前側の合成。
    }
    void SceneLight::Update()
    {
        for (auto& pt : m_light.pointLights) {
            pt.Update();
        }
        for (auto& sp : m_light.spotLights) {
            sp.Update();
        }
        m_light.numPointLight = MAX_POINT_LIGHT - static_cast<int>(m_unusePointLightQueue.size());
        m_light.numSpotLight = MAX_SPOT_LIGHT - static_cast<int>(m_unuseSpotLightQueue.size());
    }
}