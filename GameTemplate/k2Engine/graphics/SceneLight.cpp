#include "k2EnginePreCompile.h"
#include "SceneLight.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
    void PointLight::Update()
    {
        // �g�p���̃��C�g�̓J������Ԃł̍��W���v�Z����B
        if (!isUse) {
            return;
        }
        positionInView = position;
        g_camera3D->GetViewMatrix().Apply(positionInView);
    }
    void SpotLight::Update()
    {
        // �g�p���̃��C�g�̓J������Ԃł̍��W���v�Z����B
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
        // ���z��
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

        // �n�ʂ���̏Ƃ�Ԃ�
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

        // �S�Ẵ|�C���g���C�g�𖢎g�p�ɂ���
        for (auto& pt : m_light.pointLights) {
            pt.UnUse();
            pt.SetAffectPowParam(1.0f);
            m_unusePointLightQueue.push_back(&pt);
        }
        // ���ׂẴX�|�b�g���C�g�𖢎g�p�ɂ���B
        for (auto& sp : m_light.spotLights) {
            sp.UnUse();
            m_unuseSpotLightQueue.push_back(&sp);
        }
        
        m_volumeLightMapFront.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapFormat.colorBufferFormat,
            g_drawVolumeLightMapFormat.depthBufferFormat
        );
        m_volumeLightMapBack.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapFormat.colorBufferFormat,
            g_drawVolumeLightMapFormat.depthBufferFormat
        );

    }
    void SceneLight::SetIBLTextureForAmbient(const wchar_t* textureFilePath, float luminance)
    {
        g_renderingEngine->ReInitIBL(textureFilePath, luminance);
    }
    void SceneLight::DrawToVulumeLightMap(RenderContext& rc)
    {
        // �{�����[�����C�g�̔w�ʂ�`��
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapBack);
        rc.SetRenderTargetAndViewport(m_volumeLightMapBack);
        rc.SetRenderTarget(
            m_volumeLightMapBack.GetRTVCpuDescriptorHandle(),
            m_gBuffer[enGBufferAlbedoDepth].GetDSVCpuDescriptorHandle()
        );
        rc.ClearRenderTargetView(m_volumeLightMapBack);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMapBack(rc);
        }
        // �{�����[�����C�g�̑O�ʂ�`��B
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapFront);
        rc.SetRenderTargetAndViewport(m_volumeLightMapFront);
        rc.ClearRenderTargetView(m_volumeLightMapFront);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMapFront(rc);
        }

        // ���Ǝ�O�̏������݊����҂�
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapBack);
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapFront);

        // todo �����͂������牜���Ǝ�O���̍����B
    }
    void SceneLight::DebugDraw(RenderContext& rc)
    {
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMapFront(rc);
        }
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