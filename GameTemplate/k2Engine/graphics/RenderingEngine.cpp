#include "k2EnginePreCompile.h"
#include "RenderingEngine.h"


RenderingEngine* g_renderingEngine = nullptr;
SceneLight* g_sceneLight = nullptr;
void RenderingEngine::Init(bool isSoftShadow)
{
    m_isSoftShadow = isSoftShadow;

    m_sceneMaxPosition = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    m_sceneMinPosition = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    InitZPrepassRenderTarget();
    InitMainRenderTarget();
    InitGBuffer();
    InitMainRTSnapshotRenderTarget();
    InitCopyMainRenderTargetToFrameBufferSprite();
    InitShadowMapRender();
    InitDeferredLighting();
    m_lightCulling.Init(
        m_zprepassRenderTarget.GetRenderTargetTexture(),
        m_diferredLightingSprite.GetExpandConstantBufferGPU(),
        m_pointLightNoListInTileUAV
    );
    m_postEffect.Init(
        m_mainRenderTarget, 
        m_zprepassRenderTarget, 
        m_gBuffer[enGBufferNormal],
        m_gBuffer[enGBufferMetaricSmooth],
        m_gBuffer[enGBufferAlbedo]);
    // シーンライト
    g_sceneLight = &m_sceneLight;
}
void RenderingEngine::InitShadowMapRender()
{
    // シャドウマップの描画処理の初期化
    for (auto& shadowMapRender : m_shadowMapRenders) {
        shadowMapRender.Init(m_isSoftShadow);
    }
}
void RenderingEngine::InitZPrepassRenderTarget()
{
    float clearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
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
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );
}
void RenderingEngine::InitGBuffer()
{
    int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
    int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

    // アルベドカラーを出力用のレンダリングターゲットを初期化する
    m_gBuffer[enGBufferAlbedo].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );

    // 法線出力用のレンダリングターゲットを初期化する
    m_gBuffer[enGBufferNormal].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );

    // ワールド座標出力用のレンダリングターゲットを初期化する
    m_gBuffer[enGBufferWorldPos].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );

    // メタリック、スムース出力用のレンダリングターゲットを初期化する
    
    m_gBuffer[enGBufferMetaricSmooth].Create(
        frameBuffer_w,
        frameBuffer_h,
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
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

    // テクスチャはyBlurRenderTargetのカラーバッファー
    spriteInitData.m_textures[0] = &m_mainRenderTarget.GetRenderTargetTexture();

    // レンダリング先がフレームバッファーなので、解像度はフレームバッファーと同じ
    spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
    spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

    // ガンマ補正ありの2D描画のシェーダーを指定する
    spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
    spriteInitData.m_psEntryPoinFunc = "PSMain";
    spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    // 初期化オブジェクトを使って、スプライトを初期化する
    m_copyMainRtToFrameBufferSprite.Init(spriteInitData);

}
void RenderingEngine::InitDeferredLighting()
{
    // シーンライトを初期化する。
    m_sceneLight.Init();

    // タイルごとのポイントライトの番号を記録するリストのUAVを作成。
    m_pointLightNoListInTileUAV.Init(
        sizeof(int),
        MAX_POINT_LIGHT * NUM_TILE,
        nullptr);

    // ポストエフェクト的にディファードライティングを行うためのスプライトを初期化
    SpriteInitData spriteInitData;

    // 画面全体にレンダリングするので幅と高さはフレームバッファーの幅と高さと同じ
    spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
    spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

    // ディファードライティングで使用するテクスチャを設定
    int texNo = 0;
    for (auto& gBuffer : m_gBuffer)
    {
        spriteInitData.m_textures[texNo++] = &gBuffer.GetRenderTargetTexture();
    }

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
            spriteInitData.m_textures[texNo++]
                = &m_shadowMapRenders[i].GetShadowMap(areaNo);
        }
    }
    spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    // 初期化データを使ってスプライトを作成
    m_diferredLightingSprite.Init(spriteInitData);
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
void RenderingEngine::Execute(RenderContext& rc)
{
    // シーンのジオメトリ情報を構築。
    m_isBuildSceneInfo = false;
    for (auto renderer : m_renderObjects)
    {
        if (renderer->IsShadowCaster()) {
            bool isGetAabb;
            Vector3 vMin, vMax;
            renderer->GetAABB(vMax, vMin, isGetAabb);
            m_sceneMaxPosition.Max(vMax);
            m_sceneMinPosition.Min(vMin);
            if (isGetAabb) {
                m_isBuildSceneInfo = true;
            }
        }
    }

    // ビューカリング用のビュープロジェクション行列の計算。
    CalcViewProjectionMatrixForViewCulling();

    // シーンライトの更新。
    m_sceneLight.Update();

    // シーンライトのデータをコピー。
    m_deferredLightingCB.m_light = m_sceneLight.GetSceneLight();

    // シャドウマップへの描画
    RenderToShadowMap(rc);

    // ZPrepass
    ZPrepass(rc);

    // ライトカリング
    m_lightCulling.Execute(rc);

    // G-Bufferへのレンダリング
    RenderToGBuffer(rc);

    // ディファードライティング
    DeferredLighting(rc);

    // ディファードライティングが終わった時点でスナップショットを撮影する
    SnapshotMainRenderTarget(rc, EnMainRTSnapshot::enDrawnOpacity);

    // フォワードレンダリング
    ForwardRendering(rc);

    // ポストエフェクトを実行
    m_postEffect.Render(rc, m_mainRenderTarget);

    // 2D描画
    Render2D(rc);

    // メインレンダリングターゲットの内容をフレームバッファにコピー
    CopyMainRenderTargetToFrameBuffer(rc);

    // 登録されている描画オブジェクトをクリア
    m_renderObjects.clear();
}

void RenderingEngine::RenderToShadowMap(RenderContext& rc)
{
    if (m_isBuildSceneInfo == false) {
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
                m_sceneMaxPosition,
                m_sceneMinPosition
            );
        }
        ligNo++;
    }
}

void RenderingEngine::ZPrepass(RenderContext& rc)
{
    // まず、レンダリングターゲットとして設定できるようになるまで待つ
    rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget);

    // レンダリングターゲットを設定
    rc.SetRenderTargetAndViewport(m_zprepassRenderTarget);

    // レンダリングターゲットをクリア
    rc.ClearRenderTargetView(m_zprepassRenderTarget);

    for (auto& renderObj : m_renderObjects) {
        renderObj->OnZPrepass(rc);
    }

    rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget);
}
void RenderingEngine::Render2D(RenderContext& rc)
{
    // レンダリングターゲットとして利用できるまで待つ。
    //PRESENTからRENDERTARGETへ。
    rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
    for (auto& renderObj : m_renderObjects) {
        renderObj->OnRender2D(rc);
    }
    //RENDERTARGETからPRESENTへ。
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

    // メインレンダリングターゲットへの書き込み終了待ち
    rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
}
void RenderingEngine::RenderToGBuffer(RenderContext& rc)
{
    // レンダリングターゲットをG-Bufferに変更
    RenderTarget* rts[enGBufferNum] = {
        &m_gBuffer[enGBufferAlbedo],        // 0番目のレンダリングターゲット
        &m_gBuffer[enGBufferNormal],        // 1番目のレンダリングターゲット
        &m_gBuffer[enGBufferWorldPos],      // 2番目のレンダリングターゲット
        &m_gBuffer[enGBufferMetaricSmooth], // 3番目のレンダリングターゲット
        &m_gBuffer[enGBUfferShadowParam],   // 4番目のレンダリングターゲット
    };

    // まず、レンダリングターゲットとして設定できるようになるまで待つ
    rc.WaitUntilToPossibleSetRenderTargets(ARRAYSIZE(rts), rts);

    // レンダリングターゲットを設定
    rc.SetRenderTargets(ARRAYSIZE(rts), rts);

    // レンダリングターゲットをクリア
    rc.ClearRenderTargetViews(ARRAYSIZE(rts), rts);

    for (auto& renderObj : m_renderObjects) {
        renderObj->OnRenderToGBuffer(rc);
    }

    // レンダリングターゲットへの書き込み待ち
    rc.WaitUntilFinishDrawingToRenderTargets(ARRAYSIZE(rts), rts);
}

void RenderingEngine::SnapshotMainRenderTarget(RenderContext& rc, EnMainRTSnapshot enSnapshot)
{
    // メインレンダリングターゲットの内容をスナップショット
    rc.WaitUntilToPossibleSetRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
    rc.SetRenderTargetAndViewport(m_mainRTSnapshots[(int)enSnapshot]);
    m_copyMainRtToFrameBufferSprite.Draw(rc);
    rc.WaitUntilFinishDrawingToRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
}

void RenderingEngine::DeferredLighting(RenderContext& rc)
{
    // ディファードライティングに必要なライト情報を更新する
    m_deferredLightingCB.m_light.eyePos = g_camera3D->GetPosition();
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
    {
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            m_deferredLightingCB.mlvp[i][areaNo] = m_shadowMapRenders[i].GetLVPMatrix(areaNo);
        }
    }

    // レンダリング先をメインレンダリングターゲットにする
    // メインレンダリングターゲットを設定
    rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
    rc.SetRenderTargetAndViewport(m_mainRenderTarget);

    // G-Bufferの内容を元にしてディファードライティング
    m_diferredLightingSprite.Draw(rc);

    // メインレンダリングターゲットへの書き込み終了待ち
    rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
}

void RenderingEngine::CopyMainRenderTargetToFrameBuffer(RenderContext& rc)
{
    // メインレンダリングターゲットの絵をフレームバッファーにコピー
    rc.SetRenderTarget(
        g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
        g_graphicsEngine->GetCurrentFrameBuffuerDSV()
    );

    // ビューポートを指定する
    D3D12_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = g_graphicsEngine->GetFrameBufferWidth();
    viewport.Height = g_graphicsEngine->GetFrameBufferHeight();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    rc.SetViewportAndScissor(viewport);
    m_copyMainRtToFrameBufferSprite.Draw(rc);
}
