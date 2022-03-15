#include "k2EnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsK2Engine {

    RenderingEngine::RenderingEngine()
    {
        // シーンライト
        g_sceneLight = &m_sceneLight;
    }
    RenderingEngine::~RenderingEngine()
    {
        g_sceneLight = nullptr;
    }
    void RenderingEngine::Init(bool isSoftShadow)
    {
        m_isSoftShadow = isSoftShadow;

        InitZPrepassRenderTarget();
        InitMainRenderTarget();
        m_volumeLightRender.Init();
        InitGBuffer();
        InitMainRTSnapshotRenderTarget();
        InitCopyMainRenderTargetToFrameBufferSprite();
        InitShadowMapRender();
        InitDeferredLighting();
        Init2DRenderTarget();
        
        m_lightCulling.Init(
            m_zprepassRenderTarget.GetRenderTargetTexture(),
            m_diferredLightingSprite.GetExpandConstantBufferGPU(),
            m_pointLightNoListInTileUAV,
            m_spotLightNoListInTileUAV
        );
        m_postEffect.Init(
            m_mainRenderTarget,
            m_zprepassRenderTarget,
            m_gBuffer[enGBufferNormal],
            m_gBuffer[enGBufferMetaricShadowSmooth],
            m_gBuffer[enGBufferAlbedoDepth]);
        
    }
    void RenderingEngine::InitDefferedLighting_Sprite()
    {
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
        // IBL用のテクスチャをセット。

        spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
        if (m_isSoftShadow) {
            spriteInitData.m_psEntryPoinFunc = "PSMainSoftShadow";
        }
        else {
            spriteInitData.m_psEntryPoinFunc = "PSMainHardShadow";
        }
        spriteInitData.m_expandConstantBuffer = &m_deferredLightingCB;
        spriteInitData.m_expandConstantBufferSize = sizeof(m_deferredLightingCB);
        spriteInitData.m_expandShaderResoruceView[0] = &m_pointLightNoListInTileUAV;
        spriteInitData.m_expandShaderResoruceView[1] = &m_spotLightNoListInTileUAV;
        if (g_graphicsEngine->IsPossibleRaytracing()) {
            // レイトレを行うことが可能。
            spriteInitData.m_expandShaderResoruceView[2] = &g_graphicsEngine->GetRaytracingOutputTexture();
            spriteInitData.m_expandShaderResoruceView[3] = &m_giTextureBlur[eGITextureBlur_1024x1024].GetBokeTexture();
            spriteInitData.m_expandShaderResoruceView[4] = &m_giTextureBlur[eGITextureBlur_512x512].GetBokeTexture();
            spriteInitData.m_expandShaderResoruceView[5] = &m_giTextureBlur[eGITextureBlur_256x256].GetBokeTexture();
            spriteInitData.m_expandShaderResoruceView[6] = &m_giTextureBlur[eGITextureBlur_128x128].GetBokeTexture();
        }

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
            m_deferredLightingCB.m_iblLuminance = m_iblData.m_intencity;
        }

        spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        // 初期化データを使ってスプライトを作成
        m_diferredLightingSprite.Init(spriteInitData);
    }
    void RenderingEngine::ReInitIBL(const wchar_t* iblTexFilePath, float luminance)
    {
        // IBLデータを初期化。
        InitIBLData(iblTexFilePath, luminance);
        
        InitDefferedLighting_Sprite();

        m_lightCulling.Init(
            m_zprepassRenderTarget.GetRenderTargetTexture(),
            m_diferredLightingSprite.GetExpandConstantBufferGPU(),
            m_pointLightNoListInTileUAV,
            m_spotLightNoListInTileUAV
        );
        // イベントリスナーにIBLデータに変更があったことを通知する。
        for (auto& listener : m_eventListeners) {
            listener.listenerFunc(enEventReInitIBLTexture);
        }
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
            g_mainRenderTargetFormat.colorBufferFormat,
            g_mainRenderTargetFormat.depthBufferFormat
        );
    }
    void RenderingEngine::InitGBuffer()
    {
        int frameBuffer_w = g_graphicsEngine->GetFrameBufferWidth();
        int frameBuffer_h = g_graphicsEngine->GetFrameBufferHeight();

        // アルベドカラーを出力用のレンダリングターゲットを初期化する
        float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
        m_gBuffer[enGBufferAlbedoDepth].Create(
            frameBuffer_w,
            frameBuffer_h,
            1,
            1,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            DXGI_FORMAT_D32_FLOAT,
            clearColor
        );

        // 法線出力用のレンダリングターゲットを初期化する
        m_gBuffer[enGBufferNormal].Create(
            frameBuffer_w,
            frameBuffer_h,
            1,
            1,
            DXGI_FORMAT_R8G8B8A8_SNORM,
            DXGI_FORMAT_UNKNOWN
        );


        // メタリック、影パラメータ、スムース出力用のレンダリングターゲットを初期化する    
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
    void RenderingEngine::InitIBLData(const wchar_t* iblTexFilePath, float intencity)
    {
        m_iblData.m_texture.InitFromDDSFile(iblTexFilePath);
        m_iblData.m_intencity = intencity;
        g_graphicsEngine->SetRaytracingSkyCubeBox(m_iblData.m_texture);
    }
    void RenderingEngine::InitDeferredLighting()
    {
        // GIテクスチャを作成するためのブラー処理を初期化する。
        m_giTextureBlur[eGITextureBlur_1024x1024].Init(&g_graphicsEngine->GetRaytracingOutputTexture(), 1024, 1024);
        m_giTextureBlur[eGITextureBlur_512x512].Init(&m_giTextureBlur[eGITextureBlur_1024x1024].GetBokeTexture(), 512, 512);
        m_giTextureBlur[eGITextureBlur_256x256].Init(&m_giTextureBlur[eGITextureBlur_512x512].GetBokeTexture(), 256, 256);
        m_giTextureBlur[eGITextureBlur_128x128].Init(&m_giTextureBlur[eGITextureBlur_256x256].GetBokeTexture(), 128, 128);

        // シーンライトを初期化する。
        m_sceneLight.Init();

        // タイルごとのポイントライトの番号を記録するリストのUAVを作成。
        m_pointLightNoListInTileUAV.Init(
            sizeof(int),
            MAX_POINT_LIGHT * NUM_TILE,
            nullptr,
            false
        );
        // タイルごとのスポットライトの番号を記憶するリストのUAVを作成。
        m_spotLightNoListInTileUAV.Init(
            sizeof(int),
            MAX_SPOT_LIGHT * NUM_TILE,
            nullptr,
            false
        );
        // ポストエフェクト的にディファードライティングを行うためのスプライトを初期化
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
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_UNKNOWN,
            clearColor
        );

        // 最終合成用のスプライトを初期化する
        SpriteInitData spriteInitData;
        //テクスチャは2Dレンダ―ターゲット。
        spriteInitData.m_textures[0] = &m_2DRenderTarget.GetRenderTargetTexture();
        // 解像度はmainRenderTargetの幅と高さ
        spriteInitData.m_width = m_mainRenderTarget.GetWidth();
        spriteInitData.m_height = m_mainRenderTarget.GetHeight();
        // 2D用のシェーダーを使用する
        spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
        spriteInitData.m_vsEntryPointFunc = "VSMain";
        spriteInitData.m_psEntryPoinFunc = "PSMain";
        //上書き。
        spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
        //レンダリングターゲットのフォーマット。
        spriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget.GetColorBufferFormat();

        m_2DSprite.Init(spriteInitData);

        //テクスチャはメインレンダ―ターゲット。
        spriteInitData.m_textures[0] = &m_mainRenderTarget.GetRenderTargetTexture();

        //解像度は2Dレンダ―ターゲットの幅と高さ
        spriteInitData.m_width = m_2DRenderTarget.GetWidth();
        spriteInitData.m_height = m_2DRenderTarget.GetHeight();
        //レンダリングターゲットのフォーマット。
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
        // ビューカリング用のビュープロジェクション行列の計算。
        CalcViewProjectionMatrixForViewCulling();
        // シーンのジオメトリ情報の更新。
        m_sceneGeometryData.Update();
        // シーンライトの更新。
        m_sceneLight.Update();
    }
    void RenderingEngine::ComputeAnimatedVertex(RenderContext& rc)
    {
        for (auto& renderObj : m_renderObjects) {
            renderObj->OnComputeVertex(rc);
        }
    }
    void RenderingEngine::Execute(RenderContext& rc)
    {
        // シーンライトのデータをコピー。
        m_deferredLightingCB.m_light = m_sceneLight.GetSceneLight();
        m_deferredLightingCB.m_isEnableRaytracing = IsEnableRaytracing() ? 1 : 0;

        // レイトレ用のライトデータをコピー。
        m_raytracingLightData.m_directionalLight = m_sceneLight.GetSceneLight().directionalLight[0];
        m_raytracingLightData.m_iblIntencity = m_iblData.m_intencity;
        m_raytracingLightData.m_ambientLight = m_sceneLight.GetSceneLight().ambinetLight;
        m_raytracingLightData.m_enableIBLTexture = m_iblData.m_texture.IsValid() ? 1 : 0;

        // アニメーション済み頂点の計算。
        ComputeAnimatedVertex(rc);

        // シャドウマップへの描画
        RenderToShadowMap(rc);

        // ZPrepass
        ZPrepass(rc);

        // ライトカリング
        m_lightCulling.Execute(rc);

        // G-Bufferへのレンダリング
        RenderToGBuffer(rc);

        // レイトレで映り込み画像を作成する。
        if (IsEnableRaytracing()) {
            g_graphicsEngine->DispatchRaytracing(rc);
            for (auto& blur : m_giTextureBlur) {
                blur.ExecuteOnGPU(rc, 5);
            }
        }

        // ディファードライティング
        DeferredLighting(rc);
            
        // 不透明オブジェクトの描画が終わった時点でスナップショットを撮影する
        SnapshotMainRenderTarget(rc, EnMainRTSnapshot::enDrawnOpacity);

        // フォワードレンダリング
        ForwardRendering(rc);

        // ポストエフェクトを実行
        m_postEffect.Render(rc, m_mainRenderTarget);

        // 2D描画
        Render2D(rc);

        // メインレンダリングターゲットの内容をフレームバッファにコピー
        CopyMainRenderTargetToFrameBuffer(rc);
#ifdef COPY_RAYTRACING_FRAMEBUFFER
        g_graphicsEngine->DispatchRaytracing(rc);
        //レイトレの結果をフレームバッファに書き戻す。
        g_graphicsEngine->CopyToFrameBuffer(rc, g_graphicsEngine->GetRaytracingOutputTexture().Get());
#endif
        // 登録されている描画オブジェクトをクリア
        m_renderObjects.clear();
    }

    void RenderingEngine::RenderToShadowMap(RenderContext& rc)
    {
        if (m_sceneGeometryData.IsBuildshadowCasterGeometryData() == false) {
            return;
        }
        BeginGPUEvent("RenderToShadowMap");
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
        EndGPUEvent();
    }

    void RenderingEngine::ZPrepass(RenderContext& rc)
    {
        BeginGPUEvent("ZPrepass");
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
        EndGPUEvent();
    }
    void RenderingEngine::Render2D(RenderContext& rc)
    {
        BeginGPUEvent("Render2D");
        // レンダリングターゲットとして利用できるまで待つ。
        //PRESENTからRENDERTARGETへ。
        rc.WaitUntilToPossibleSetRenderTarget(m_2DRenderTarget);
    
        // レンダリングターゲットを設定
        rc.SetRenderTargetAndViewport(m_2DRenderTarget);

        // レンダリングターゲットをクリア
        rc.ClearRenderTargetView(m_2DRenderTarget);

        m_mainSprite.Draw(rc);

        for (auto& renderObj : m_renderObjects) {
            renderObj->OnRender2D(rc);
        }

        //RENDERTARGETからPRESENTへ。
        rc.WaitUntilFinishDrawingToRenderTarget(m_2DRenderTarget);
        //PRESENTからRENDERTARGETへ。
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);

        // レンダリングターゲットを設定
        rc.SetRenderTargetAndViewport(m_mainRenderTarget);

        m_2DSprite.Draw(rc);

        //RENDERTARGETからPRESENTへ。
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);

        EndGPUEvent();
    }
    void RenderingEngine::ForwardRendering(RenderContext& rc)
    {
        BeginGPUEvent("ForwardRendering");
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc.SetRenderTarget(
            m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
            m_gBuffer[enGBufferAlbedoDepth].GetDSVCpuDescriptorHandle()
        );
        
        for (auto& renderObj : m_renderObjects) {
            renderObj->OnForwardRender(rc);
        }

        // ボリュームライトを描画。
        m_volumeLightRender.Render(
            rc, 
            m_mainRenderTarget.GetRTVCpuDescriptorHandle(),
            m_gBuffer[enGBufferAlbedoDepth].GetDSVCpuDescriptorHandle()
        );

        // 続いて半透明オブジェクトを描画。
        for (auto& renderObj : m_renderObjects) {
            renderObj->OnTlanslucentRender(rc);
        }

        // メインレンダリングターゲットへの書き込み終了待ち
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
        
        EndGPUEvent();
    }
    void RenderingEngine::RenderToGBuffer(RenderContext& rc)
    {
        BeginGPUEvent("RenderToGBuffer");
        // レンダリングターゲットをG-Bufferに変更
        RenderTarget* rts[enGBufferNum] = {
            &m_gBuffer[enGBufferAlbedoDepth],         // 0番目のレンダリングターゲット
            &m_gBuffer[enGBufferNormal],              // 1番目のレンダリングターゲット
            &m_gBuffer[enGBufferMetaricShadowSmooth], // 2番目のレンダリングターゲット
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
        EndGPUEvent();
    }

    void RenderingEngine::SnapshotMainRenderTarget(RenderContext& rc, EnMainRTSnapshot enSnapshot)
    {
        BeginGPUEvent("SnapshotMainRenderTarget");

        // メインレンダリングターゲットの内容をスナップショット
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);
        rc.SetRenderTargetAndViewport(m_mainRTSnapshots[(int)enSnapshot]);
        m_copyMainRtToFrameBufferSprite.Draw(rc);
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRTSnapshots[(int)enSnapshot]);

        EndGPUEvent();
    }

    void RenderingEngine::DeferredLighting(RenderContext& rc)
    {
        BeginGPUEvent("DeferredLighting");

        // ディファードライティングに必要なライト情報を更新する
        m_deferredLightingCB.m_light.eyePos = g_camera3D->GetPosition();
        for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
        {
            for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
            {
                m_deferredLightingCB.mlvp[i][areaNo] = m_shadowMapRenders[i].GetLVPMatrix(areaNo);
            }
        }
        m_deferredLightingCB.m_light.mViewProjInv.Inverse(g_camera3D->GetViewProjectionMatrix());

        // レンダリング先をメインレンダリングターゲットにする
        // メインレンダリングターゲットを設定
        rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
        rc.SetRenderTargetAndViewport(m_mainRenderTarget);

        // G-Bufferの内容を元にしてディファードライティング
        m_diferredLightingSprite.Draw(rc);

        // メインレンダリングターゲットへの書き込み終了待ち
        rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);

        EndGPUEvent();
    }

    void RenderingEngine::CopyMainRenderTargetToFrameBuffer(RenderContext& rc)
    {
        BeginGPUEvent("CopyMainRenderTargetToFrameBuffer");

        // メインレンダリングターゲットの絵をフレームバッファーにコピー
        rc.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );

        // ビューポートを指定する
        D3D12_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<FLOAT>(g_graphicsEngine->GetFrameBufferWidth());
        viewport.Height = static_cast<FLOAT>(g_graphicsEngine->GetFrameBufferHeight());
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        rc.SetViewportAndScissor(viewport);
        m_copyMainRtToFrameBufferSprite.Draw(rc);

        EndGPUEvent();
    }
}