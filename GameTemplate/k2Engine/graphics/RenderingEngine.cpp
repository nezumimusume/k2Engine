#include "k2EnginePreCompile.h"
#include "RenderingEngine.h"


RenderingEngine* g_renderingEngine = nullptr;

void RenderingEngine::Init()
{
    InitZPrepassRenderTarget();
    InitMainRenderTarget();
    InitGBuffer();
    InitMainRTSnapshotRenderTarget();
    InitCopyMainRenderTargetToFrameBufferSprite();
    InitShadowMapRender();
    InitDeferredLighting();
    m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget);
}
void RenderingEngine::InitShadowMapRender()
{
    // �V���h�E�}�b�v�̕`�揈���̏�����
    for (auto& shadowMapRender : m_shadowMapRenders) {
        shadowMapRender.Init();
    }
}
void RenderingEngine::InitZPrepassRenderTarget()
{
    m_zprepassRenderTarget.Create(
        g_graphicsEngine->GetFrameBufferWidth(),
        g_graphicsEngine->GetFrameBufferHeight(),
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
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
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );
}
void RenderingEngine::InitGBuffer()
{
    int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
    int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

    // �A���x�h�J���[���o�͗p�̃����_�����O�^�[�Q�b�g������������
    m_gBuffer[enGBufferAlbedo].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );

    // �@���o�͗p�̃����_�����O�^�[�Q�b�g������������
    // todo DXGI_FORMAT_R32G32B32A32_FLOAT���̐��x�͕s�v�Ȃ̂ŁA��ŁADXGI_FORMAT_R8G8B8A8_UNORM�ɕύX����
    m_gBuffer[enGBufferNormal].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );

    // ���^���b�N�A�X���[�X�o�͗p�̃����_�����O�^�[�Q�b�g������������
    // todo DXGI_FORMAT_R32G32B32A32_FLOAT���̐��x�͕s�v�Ȃ̂ŁA��ŁADXGI_FORMAT_R8G8B8A8_UNORM�ɕύX����
    m_gBuffer[enGBufferMetaricSmooth].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );

    // ���[���h���W�o�͗p�̃����_�����O�^�[�Q�b�g������������
    // todo DXGI_FORMAT_R32G32B32A32_FLOAT���̐��x�͕s�v�Ȃ̂ŁA
    // ���ƂŁADXGI_FORMAT_R8G8B8A8_UNORM�ɕύX����
    m_gBuffer[enGBufferWorldPos].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );
    m_gBuffer[enGBUfferShadowParam].Create(
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

    // �{�P�摜�����̂܂ܓ\��t���邾���Ȃ̂ŁA�ʏ��2D�`��̃V�F�[�_�[���w�肷��
    spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
    spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    // �������I�u�W�F�N�g���g���āA�X�v���C�g������������
    m_copyMainRtToFrameBufferSprite.Init(spriteInitData);

}
void RenderingEngine::InitDeferredLighting()
{
    // �V�[�����C�g������������B
    m_sceneLight.Init();

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

    spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
    spriteInitData.m_expandConstantBuffer = &m_deferredLightingCB;
    spriteInitData.m_expandConstantBufferSize = sizeof(m_deferredLightingCB);
    for (int i = 0; i < NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT; i++)
    {
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            spriteInitData.m_textures[texNo++]
                = &m_shadowMapRenders[i].GetShadowMap(areaNo);
        }
    }
    spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    // �������f�[�^���g���ăX�v���C�g���쐬
    m_diferredLightingSprite.Init(spriteInitData);
}
void RenderingEngine::Execute(RenderContext& rc)
{
    // �V�[�����C�g�̃f�[�^���R�s�[�B
    m_deferredLightingCB.m_light = m_sceneLight.GetSceneLight();

    // �V���h�E�}�b�v�ւ̕`��
    RenderToShadowMap(rc);

    // ZPrepass
    ZPrepass(rc);

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
    int ligNo = 0;
    for (auto& shadowMapRender : m_shadowMapRenders)
    {
        shadowMapRender.Render(
            rc,
            ligNo,
            m_deferredLightingCB.m_light.directionalLight[ligNo].direction,
            m_renderObjects
        );
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
    rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
    for (auto& renderObj : m_renderObjects) {
        renderObj->OnRender2D(rc);
    }
    //RENDERTARGET����PRESENT�ցB
    rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
    
}
void RenderingEngine::ForwardRendering(RenderContext& rc)
{
    rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
    rc.SetRenderTarget(
        m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
        m_gBuffer[enGBufferAlbedo].GetDSVCpuDescriptorHandle()
    );
    for (auto& renderObj : m_renderObjects) {
        renderObj->OnForwardRender(rc);
    }

    // ���C�������_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
    rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
}
void RenderingEngine::RenderToGBuffer(RenderContext& rc)
{
    // �����_�����O�^�[�Q�b�g��G-Buffer�ɕύX
    RenderTarget* rts[enGBufferNum] = {
        &m_gBuffer[enGBufferAlbedo],        // 0�Ԗڂ̃����_�����O�^�[�Q�b�g
        &m_gBuffer[enGBufferNormal],        // 1�Ԗڂ̃����_�����O�^�[�Q�b�g
        &m_gBuffer[enGBufferWorldPos],      // 2�Ԗڂ̃����_�����O�^�[�Q�b�g
        &m_gBuffer[enGBufferMetaricSmooth], // 3�Ԗڂ̃����_�����O�^�[�Q�b�g
        &m_gBuffer[enGBUfferShadowParam],   // 4�Ԗڂ̃����_�����O�^�[�Q�b�g
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
    for (int i = 0; i < NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT; i++)
    {
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            m_deferredLightingCB.mlvp[i][areaNo] = m_shadowMapRenders[i].GetLVPMatrix(areaNo);
        }
    }

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
    viewport.Width = 1280;
    viewport.Height = 720;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    rc.SetViewportAndScissor(viewport);
    m_copyMainRtToFrameBufferSprite.Draw(rc);
}
