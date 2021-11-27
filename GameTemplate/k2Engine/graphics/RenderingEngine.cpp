#include "k2EnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsK2Engine {

    void RenderingEngine::Init(bool isSoftShadow)
    {
        m_isSoftShadow = isSoftShadow;

        InitZPrepassRenderTarget();
        InitMainRenderTarget();
        InitGBuffer();
        InitMainRTSnapshotRenderTarget();
        InitCopyMainRenderTargetToFrameBufferSprite();
        InitShadowMapRender();
        InitDeferredLighting();
        Init2DRenderTarget();
        m_lightCulling.Init(
            m_zprepassRenderTarget.GetRenderTargetTexture(),
            m_diferredLightingSprite.GetExpandConstantBufferGPU(),
            m_pointLightNoListInTileUAV
        );
        m_postEffect.Init(
            m_mainRenderTarget,
            m_zprepassRenderTarget,
            m_gBuffer[enGBufferNormal],
            m_gBuffer[enGBufferMetaricShadowSmooth],
            m_gBuffer[enGBufferAlbedoDepth]);
        // �V�[�����C�g
        g_sceneLight = &m_sceneLight;
    }
    void RenderingEngine::InitDefferedLighting_Sprite()
    {
        // �|�X�g�G�t�F�N�g�I�Ƀf�B�t�@�[�h���C�e�B���O���s�����߂̃X�v���C�g��������
        SpriteInitData spriteInitData;

        // ��ʑS�̂Ƀ����_�����O����̂ŕ��ƍ����̓t���[���o�b�t�@�[�̕��ƍ����Ɠ���
        spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
        spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

        // �f�B�t�@�[�h���C�e�B���O�Ŏg�p����e�N�X�`����ݒ�
        int texNo = 0;
        for (auto& gBuffer : m_gBuffer)
        {
            spriteInitData.m_textures[texNo++] = &gBuffer.GetRenderTargetTexture();
        }
        // IBL�p�̃e�N�X�`�����Z�b�g�B

        spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
        if (m_isSoftShadow) {
            spriteInitData.m_psEntryPoinFunc = "PSMainSoftShadow";
        }
        else {
            spriteInitData.m_psEntryPoinFunc = "PSMainHardShadow";
        }
        spriteInitData.m_expandConstantBuffer = &m_deferredLightingCB;
        spriteInitData.m_expandConstantBufferSize = sizeof(m_deferredLightingCB);
        spriteInitData.m_expandShaderResoruceView = &m_pointLightNoListInTileUAV;

        for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
        {
            for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
            {
                spriteInitData.m_textures[texNo++] = &m_shadowMapRenders[i].GetShadowMap(areaNo);
            }
        }
        if (m_iblData.m_texture.IsValid()) {
            spriteInitData.m_textures[texNo++] = &m_iblData.m_texture;
            m_deferredLightingCB.m_isIBL = 1;
            m_deferredLightingCB.m_iblLuminance = m_iblData.m_luminance;
        }

        spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        // �������f�[�^���g���ăX�v���C�g���쐬
        m_diferredLightingSprite.Init(spriteInitData);
    }
    void RenderingEngine::ReInitIBL(const wchar_t* iblTexFilePath, float luminance)
    {
        // IBL�f�[�^���������B
        InitIBLData(iblTexFilePath, luminance);
        
        InitDefferedLighting_Sprite();

        m_lightCulling.Init(
            m_zprepassRenderTarget.GetRenderTargetTexture(),
            m_diferredLightingSprite.GetExpandConstantBufferGPU(),
            m_pointLightNoListInTileUAV
        );
        // �C�x���g���X�i�[��IBL�f�[�^�ɕύX�����������Ƃ�ʒm����B
        for (auto& listener : m_eventListeners) {
            listener.listenerFunc(enEventReInitIBLTexture);
        }
    }
    void RenderingEngine::InitShadowMapRender()
    {
        // �V���h�E�}�b�v�̕`�揈���̏�����
        for (auto& shadowMapRender : m_shadowMapRenders) {
            shadowMapRender.Init(m_isSoftShadow);
        }
    }
    void RenderingEngine::InitZPrepassRenderTarget()
    {
        float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        m_zprepassRenderTarget.Create(
            g_graphicsEngine->GetFrameBufferWidth(),
            g_graphicsEngine->GetFrameBufferHeight(),
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            DXGI_FORMAT_D32_FLOAT,
            clearColor
        );
    }
    void RenderingEngine::InitMainRTSnapshotRenderTarget()
    {
        for (auto& snapshotRt : m_mainRTSnapshots) {
            snapshotRt.Create(
                g_graphicsEngine->GetFrameBufferWidth(),
                g_graphicsEngine->GetFrameBufferHeight(),
                1,
                1,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_FORMAT_UNKNOWN
            );
        }
    }
    void RenderingEngine::InitMainRenderTarget()
    {
        m_mainRenderTarget.Create(
            g_graphicsEngine->GetFrameBufferWidth(),
            g_graphicsEngine->GetFrameBufferHeight(),
            1,
            1,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            DXGI_FORMAT_UNKNOWN
        );
    }
    void RenderingEngine::InitGBuffer()
    {
        int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
        int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

        // �A���x�h�J���[���o�͗p�̃����_�����O�^�[�Q�b�g������������
        m_gBuffer[enGBufferAlbedoDepth].Create(
            frameBuffer_w,
            frameBuffer_h,
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            DXGI_FORMAT_D32_FLOAT
        );

        // �@���o�͗p�̃����_�����O�^�[�Q�b�g������������
        m_gBuffer[enGBufferNormal].Create(
            frameBuffer_w,
            frameBuffer_h,
            1,
            1,
            DXGI_FORMAT_R8G8B8A8_SNORM,
            DXGI_FORMAT_UNKNOWN
        );


        // ���^���b�N�A�e�p�����[�^�A�X���[�X�o�͗p�̃����_�����O�^�[�Q�b�g������������    
        m_gBuffer[enGBufferMetaricShadowSmooth].Create(
            frameBuffer_w,
            frameBuffer_h,
            1,
            1,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_UNKNOWN
        );

    }
    void RenderingEngine::InitCopyMainRenderTargetToFrameBufferSprite()
    {
        SpriteInitData spriteInitData;

        // �e�N�X�`����yBlurRenderTarget�̃J���[�o�b�t�@�[
        spriteInitData.m_textures[0] = &m_mainRenderTarget.GetRenderTargetTexture();

        // �����_�����O�悪�t���[���o�b�t�@�[�Ȃ̂ŁA�𑜓x�̓t���[���o�b�t�@�[�Ɠ���
        spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
        spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

        // �K���}�␳�����2D�`��̃V�F�[�_�[���w�肷��
        spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
        spriteInitData.m_psEntryPoinFunc = "PSMain";
        spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        // �������I�u�W�F�N�g���g���āA�X�v���C�g������������
        m_copyMainRtToFrameBufferSprite.Init(spriteInitData);

    }
    void RenderingEngine::InitIBLData(const wchar_t* iblTexFilePath, float luminance)
    {
        m_iblData.m_texture.InitFromDDSFile(iblTexFilePath);
        m_iblData.m_luminance = luminance;
    }
    void RenderingEngine::InitDeferredLighting()
    {
        // �V�[�����C�g������������B
        m_sceneLight.Init();

        // �^�C�����Ƃ̃|�C���g���C�g�̔ԍ����L�^���郊�X�g��UAV���쐬�B
        m_pointLightNoListInTileUAV.Init(
            sizeof(int),
            MAX_POINT_LIGHT * NUM_TILE,
            nullptr);

        // �|�X�g�G�t�F�N�g�I�Ƀf�B�t�@�[�h���C�e�B���O���s�����߂̃X�v���C�g��������
        InitDefferedLighting_Sprite();
    }
    void RenderingEngine::Init2DRenderTarget()
    {
        float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };

        m_2DRenderTarget.Create(
            UI_SPACE_WIDTH,
            UI_SPACE_HEIGHT,
            1,
            1,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            DXGI_FORMAT_UNKNOWN,
            clearColor
        );

        // �ŏI�����p�̃X�v���C�g������������
        SpriteInitData spriteInitData;
        //�e�N�X�`����2D�����_�\�^�[�Q�b�g�B
        spriteInitData.m_textures[0] = &m_2DRenderTarget.GetRenderTargetTexture();
        // �𑜓x��mainRenderTarget�̕��ƍ���
        spriteInitData.m_width = m_mainRenderTarget.GetWidth();
        spriteInitData.m_height = m_mainRenderTarget.GetHeight();
        // 2D�p�̃V�F�[�_�[���g�p����
        spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";
        //�㏑���B
        spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
        //�����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g�B
        spriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget.GetColorBufferFormat();

        m_2DSprite.Init(spriteInitData);

        //�e�N�X�`���̓��C�������_�\�^�[�Q�b�g�B
        spriteInitData.m_textures[0] = &m_mainRenderTarget.GetRenderTargetTexture();

        //�𑜓x��2D�����_�\�^�[�Q�b�g�̕��ƍ���
        spriteInitData.m_width = m_2DRenderTarget.GetWidth();
        spriteInitData.m_height = m_2DRenderTarget.GetHeight();
        //�����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g�B
        spriteInitData.m_colorBufferFormat[0] = m_2DRenderTarget.GetColorBufferFormat();
       
        m_mainSprite.Init(spriteInitData);
    }
    void RenderingEngine::CalcViewProjectionMatrixForViewCulling()
    {
        Matrix projMatrix;
        projMatrix.MakeProjectionMatrix(
            g_camera3D->GetViewAngle() * 1.5f,
            g_camera3D->GetAspect(),
            g_camera3D->GetNear(),
            g_camera3D->GetFar()
        );
        m_viewProjMatrixForViewCulling.Multiply(g_camera3D->GetViewMatrix(), projMatrix);
    }
    void RenderingEngine::Update()
    {
        // �r���[�J�����O�p�̃r���[�v���W�F�N�V�����s��̌v�Z�B
        CalcViewProjectionMatrixForViewCulling();
        // �V�[���̃W�I���g�����̍X�V�B
        m_sceneGeometryData.Update();
        // �V�[�����C�g�̍X�V�B
        m_sceneLight.Update();
    }
    void RenderingEngine::Execute(RenderContext& rc)
    {
        // �V�[�����C�g�̃f�[�^���R�s�[�B
        m_deferredLightingCB.m_light = m_sceneLight.GetSceneLight();

        // �V���h�E�}�b�v�ւ̕`��
        RenderToShadowMap(rc);

        // ZPrepass
        ZPrepass(rc);

        // ���C�g�J�����O
        m_lightCulling.Execute(rc);

        // G-Buffer�ւ̃����_�����O
        RenderToGBuffer(rc);

        // �f�B�t�@�[�h���C�e�B���O
        DeferredLighting(rc);

        // �f�B�t�@�[�h���C�e�B���O���I��������_�ŃX�i�b�v�V���b�g���B�e����
        SnapshotMainRenderTarget(rc, EnMainRTSnapshot::enDrawnOpacity);

        // �t�H���[�h�����_�����O
        ForwardRendering(rc);

        // �|�X�g�G�t�F�N�g�����s
        m_postEffect.Render(rc, m_mainRenderTarget);

        // 2D�`��
        Render2D(rc);

        // ���C�������_�����O�^�[�Q�b�g�̓��e���t���[���o�b�t�@�ɃR�s�[
        CopyMainRenderTargetToFrameBuffer(rc);

        // �o�^����Ă���`��I�u�W�F�N�g���N���A
        m_renderObjects.clear();
    }

    void RenderingEngine::RenderToShadowMap(RenderContext& rc)
    {
        if (m_sceneGeometryData.IsBuildshadowCasterGeometryData() == false) {
            return;
        }
        int ligNo = 0;
        for (auto& shadowMapRender : m_shadowMapRenders)
        {
            if (m_sceneLight.IsCastShadow(ligNo)) {
                shadowMapRender.Render(
                    rc,
                    ligNo,
                    m_deferredLightingCB.m_light.directionalLight[ligNo].direction,
                    m_renderObjects,
                    m_sceneGeometryData.GetShadowCasterMaxPositionInViewFrustum(),
                    m_sceneGeometryData.GetShadowCasterMinPositionInViewFrustum()
                );
            }
            ligNo++;
        }
    }

    void RenderingEngine::ZPrepass(RenderContext& rc)
    {
        // �܂��A�����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂�
        rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetAndViewport(m_zprepassRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        rc.ClearRenderTargetView(m_zprepassRenderTarget);

        for (auto& renderObj : m_renderObjects) {
            renderObj->OnZPrepass(rc);
        }

        rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget);
    }
    void RenderingEngine::Render2D(RenderContext& rc)
    {
        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂B
        //PRESENT����RENDERTARGET�ցB
        rc.WaitUntilToPossibleSetRenderTarget(m_2DRenderTarget);
    
        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetAndViewport(m_2DRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        rc.ClearRenderTargetView(m_2DRenderTarget);

        m_mainSprite.Draw(rc);

        for (auto& renderObj : m_renderObjects) {
            renderObj->OnRender2D(rc);
        }

        //RENDERTARGET����PRESENT�ցB
        rc.WaitUntilFinishDrawingToRenderTarget(m_2DRenderTarget);
        //PRESENT����RENDERTARGET�ցB
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargetAndViewport(m_mainRenderTarget);

        m_2DSprite.Draw(rc);

        //RENDERTARGET����PRESENT�ցB
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }
    void RenderingEngine::ForwardRendering(RenderContext& rc)
    {
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc.SetRenderTarget(
            m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
            m_gBuffer[enGBufferAlbedoDepth].GetDSVCpuDescriptorHandle()
        );
        for (auto& renderObj : m_renderObjects) {
            renderObj->OnForwardRender(rc);
        }

        // �����Ĕ������I�u�W�F�N�g��`��B
        for (auto& renderObj : m_renderObjects) {
            renderObj->OnTlanslucentRender(rc);
        }

        // ���C�������_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }
    void RenderingEngine::RenderToGBuffer(RenderContext& rc)
    {
        // �����_�����O�^�[�Q�b�g��G-Buffer�ɕύX
        RenderTarget* rts[enGBufferNum] = {
            &m_gBuffer[enGBufferAlbedoDepth],         // 0�Ԗڂ̃����_�����O�^�[�Q�b�g
            &m_gBuffer[enGBufferNormal],              // 1�Ԗڂ̃����_�����O�^�[�Q�b�g
            &m_gBuffer[enGBufferMetaricShadowSmooth], // 2�Ԗڂ̃����_�����O�^�[�Q�b�g
        };

        // �܂��A�����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂�
        rc.WaitUntilToPossibleSetRenderTargets(ARRAYSIZE(rts), rts);

        // �����_�����O�^�[�Q�b�g��ݒ�
        rc.SetRenderTargets(ARRAYSIZE(rts), rts);

        // �����_�����O�^�[�Q�b�g���N���A
        rc.ClearRenderTargetViews(ARRAYSIZE(rts), rts);

        for (auto& renderObj : m_renderObjects) {
            renderObj->OnRenderToGBuffer(rc);
        }

        // �����_�����O�^�[�Q�b�g�ւ̏������ݑ҂�
        rc.WaitUntilFinishDrawingToRenderTargets(ARRAYSIZE(rts), rts);
    }

    void RenderingEngine::SnapshotMainRenderTarget(RenderContext& rc, EnMainRTSnapshot enSnapshot)
    {
        // ���C�������_�����O�^�[�Q�b�g�̓��e���X�i�b�v�V���b�g
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
        rc.SetRenderTargetAndViewport(m_mainRTSnapshots[(int)enSnapshot]);
        m_copyMainRtToFrameBufferSprite.Draw(rc);
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
    }

    void RenderingEngine::DeferredLighting(RenderContext& rc)
    {
        // �f�B�t�@�[�h���C�e�B���O�ɕK�v�ȃ��C�g�����X�V����
        m_deferredLightingCB.m_light.eyePos = g_camera3D->GetPosition();
        for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
        {
            for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
            {
                m_deferredLightingCB.mlvp[i][areaNo] = m_shadowMapRenders[i].GetLVPMatrix(areaNo);
            }
        }
        m_deferredLightingCB.m_light.mViewProjInv.Inverse(g_camera3D->GetViewProjectionMatrix());

        // �����_�����O������C�������_�����O�^�[�Q�b�g�ɂ���
        // ���C�������_�����O�^�[�Q�b�g��ݒ�
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc.SetRenderTargetAndViewport(m_mainRenderTarget);

        // G-Buffer�̓��e�����ɂ��ăf�B�t�@�[�h���C�e�B���O
        m_diferredLightingSprite.Draw(rc);

        // ���C�������_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    }

    void RenderingEngine::CopyMainRenderTargetToFrameBuffer(RenderContext& rc)
    {
        // ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[
        rc.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );

        // �r���[�|�[�g���w�肷��
        D3D12_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(g_graphicsEngine->GetFrameBufferWidth());
        viewport.Height = static_cast<FLOAT>(g_graphicsEngine->GetFrameBufferHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        rc.SetViewportAndScissor(viewport);
        m_copyMainRtToFrameBufferSprite.Draw(rc);
    }
}