#include "k2EnginePreCompile.h"
#include "ShadowMapRender.h"

namespace nsK2Engine {
    void ShadowMapRender::Init(bool isSoftShadow)
    {
 
        float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        DXGI_FORMAT colorFormat;
        DXGI_FORMAT depthFormat;
        if (isSoftShadow) {
            colorFormat = g_softShadowMapFormat.colorBufferFormat;
            depthFormat = g_softShadowMapFormat.depthBufferFormat;
        }
        else {
            colorFormat = g_hardShadowMapFormat.colorBufferFormat;
            depthFormat = g_hardShadowMapFormat.depthBufferFormat;
        }
        //�ߌi�p�̃V���h�E�}�b�v
        m_shadowMaps[0].Create(
            2048,
            2048,
            1,
            1,
            colorFormat,
            depthFormat,
            clearColor
        );
        //���i�p�̃V���h�E�}�b�v
        m_shadowMaps[1].Create(
            1024,
            1024,
            1,
            1,
            colorFormat,
            depthFormat,
            clearColor
        );
        //���i�p�̃V���h�E�}�b�v
        m_shadowMaps[2].Create(
            512,
            512,
            1,
            1,
            colorFormat,
            depthFormat,
            clearColor
        );

        if (isSoftShadow) {
            // �\�t�g�V���h�E���s���B
            m_blur[0].Init(&m_shadowMaps[0].GetRenderTargetTexture());
            m_blur[1].Init(&m_shadowMaps[1].GetRenderTargetTexture());
            m_blur[2].Init(&m_shadowMaps[2].GetRenderTargetTexture());
        }
        m_isSoftShadow = isSoftShadow;
    }

    void ShadowMapRender::Render(
        RenderContext& rc,
        int ligNo,
        Vector3& lightDirection,
        std::vector< IRenderer* >& renderObjects,
        const Vector3& sceneMaxPosition,
        const Vector3& sceneMinPosition
    )
    {
        if (lightDirection.LengthSq() < 0.001f) {
            return;
        }
        // ���C�g�̍ő�̍����������_���[��AABB����v�Z����B
        m_cascadeShadowMapMatrix.CalcLightViewProjectionCropMatrix(
            lightDirection,
            m_cascadeAreaRateArray,
            sceneMaxPosition,
            sceneMinPosition
        );

        int shadowMapNo = 0;
        for (auto& shadowMap : m_shadowMaps) {
            rc.WaitUntilToPossibleSetRenderTarget(shadowMap);
            rc.SetRenderTargetAndViewport(shadowMap);
            rc.ClearRenderTargetView(shadowMap);

            for (auto& renderer : renderObjects) {
                renderer->OnRenderShadowMap(
                    rc,
                    ligNo,
                    shadowMapNo,
                    m_cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(shadowMapNo)
                );
            }

            //�`�悪�I�������N���A
            m_renderers.clear();

            // �������݊����҂�
            rc.WaitUntilFinishDrawingToRenderTarget(shadowMap);
            shadowMapNo++;
        }

        if (m_isSoftShadow) {
            // �u���[�����s����B
            for (auto& blur : m_blur) {
                blur.ExecuteOnGPU(rc, 1.0f);
            }
        }
    }
}