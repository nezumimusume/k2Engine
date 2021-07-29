#pragma once

#include "MyRenderer.h"
#include "ShadowMapRender.h"
#include "PostEffect.h"


class IRenderer;

/// <summary>
/// レンダリングエンジン。
/// </summary>
class RenderingEngine
{
public:
    // ディレクションライト
    struct DirectionalLight
    {
        Vector3 direction;  // ライトの方向
        int castShadow;     // 影をキャストする？
        Vector4 color;      // ライトのカラー
    };

    // ライト構造体
    struct Light
    {
        std::array<DirectionalLight,
            NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT>
            directionalLight; // ディレクションライト
        Vector3 eyePos;         // カメラの位置
        float pad;
        Vector3 ambinetLight;   // 環境光
    };

    //メインレンダリングターゲットのスナップショット
    enum class EnMainRTSnapshot
    {
        enDrawnOpacity,     // 不透明オブジェクトの描画完了時点
        enNum,              // スナップショットの数
    };

    // レンダリングパス
    enum class EnRenderingPass
    {
        enRenderToShadowMap,    //シャドウマップへの描画パス
        enZPrepass,             //ZPrepass
        enRenderToGBuffer,      //G-Bufferへの描画パス
        enForwardRender,        //フォワードレンダリングの描画パス
        enPostEffect,           //ポストエフェクト
        enRender2D,             //2D描画。
    };
    /// <summary>
    /// レンダリングパイプラインを初期化
    /// </summary>
    void Init();
    
    /// <summary>
    /// 描画オブジェクトを追加。
    /// </summary>
    /// <param name="renderObject"></param>
    void AddRenderObject(IRenderer* renderObject)
    {
        m_renderObjects.push_back(renderObject);
    }
    /// <summary>
    /// ZPrepassで作成された深度テクスチャを取得
    /// </summary>
    /// <returns></returns>
    Texture& GetZPrepassDepthTexture()
    {
        return m_zprepassRenderTarget.GetRenderTargetTexture();
    }
    /// <summary>
    /// GBufferのアルベドテクスチャを取得
    /// </summary>
    /// <returns></returns>
    Texture& GetGBufferAlbedoTexture()
    {
        return m_gBuffer[enGBufferAlbedo].GetRenderTargetTexture();
    }
    /// <summary>
    /// GBufferの法線テクスチャを取得。
    /// </summary>
    /// <returns></returns>
    Texture& GetGBufferNormalTexture()
    {
        return m_gBuffer[enGBufferNormal].GetRenderTargetTexture();
    }
    /// <summary>
    /// GBufferのワールド座標テクスチャを取得。
    /// </summary>
    /// <returns></returns>
    Texture& GetGBufferWorldTexture()
    {
        return m_gBuffer[enGBufferWorldPos].GetRenderTargetTexture();
    }
    /// <summary>
    /// 不透明オブジェクトの描画完了時のメインレンダリングターゲットの
    /// スナップショットを取得
    /// </summary>
    /// <returns></returns>
    Texture& GetMainRenderTargetSnapshotDrawnOpacity()
    {
        return m_mainRTSnapshots[(int)EnMainRTSnapshot::enDrawnOpacity].GetRenderTargetTexture();
    }
    /// <summary>
    /// レンダリングパイプラインを実行
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト。</param>
    void Execute(RenderContext& rc);
    /// <summary>
    /// ディレクションライトのパラメータを設定
    /// </summary>
    /// <param name="lightNo"></param>
    /// <param name="direction"></param>
    /// <param name="color"></param>
    void SetDirectionLight(int lightNo, Vector3 direction, Vector3 color)
    {
        m_deferredLightingCB.m_light.directionalLight[lightNo].direction = direction;
        m_deferredLightingCB.m_light.directionalLight[lightNo].color = color;
    }
    void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
    {
        rc.SetRenderTarget(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), m_gBuffer[enGBufferAlbedo].GetDSVCpuDescriptorHandle());
    }
    /// <summary>
    /// カスケードシャドウのエリア率を設定。
    /// </summary>
    /// <remark>
    /// ゲームカメラの近平面から遠平面までのエリアの、何%の範囲を近距離用の高解像度のシャドウマップに、
    /// 何%を中距離用のシャドウマップに、何%を遠距離用のシャドウマップに描画するかを指定したもの。
    /// 例えば、nearArea  = 0.1(10%)、middleArea = 0.3(30%), farArea = 0.8(80%)と指定されていたら、
    /// カメラから10%のエリアは近距離用のシャドウマップに描画、
    /// カメラから10%～30%のエリアは中距離用のシャドウマップに描画、
    /// カメラから30%～80%のエリアは遠距離用のシャドウマップに描画となる。
    /// 影の品質の調整に使ってください。
    /// </remark>
    /// <param name="nearArea">近影エリア率。0.0f～1.0f</param>
    /// <param name="nearArea">中影エリア率。0.0f～1.0f</param>
    /// <param name="nearArea">遠影エリア率。0.0f～1.0f</param>
    void  SetCascadeNearAreaRates(float nearArea, float middleArea, float farArea)
    {
        for (auto& renderer : m_shadowMapRenders) {
            renderer.SetCascadeNearAreaRates(nearArea, middleArea, farArea);
        }
    }
private:
    /// <summary>
    /// G-Bufferを初期化
    /// </summary>
    void InitGBuffer();
    /// <summary>
    /// ディファードライティングの初期化
    /// </summary>
    void InitDeferredLighting();
    /// <summary>
    /// シャドウマップに描画
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void RenderToShadowMap(RenderContext& rc);
    /// <summary>
    /// ZPrepass
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void ZPrepass(RenderContext& rc);
    /// <summary>
    /// G-Bufferへの描画
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト。</param>
    void RenderToGBuffer(RenderContext& rc);
    /// <summary>
    /// ディファードライティング
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void DeferredLighting(RenderContext& rc);
    /// <summary>
    /// 2D描画
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void Render2D(RenderContext& rc);
    /// <summary>
    /// メインレンダリングターゲットの内容をフレームバッファにコピーする
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void CopyMainRenderTargetToFrameBuffer(RenderContext& rc);
    /// <summary>
    /// フォワードレンダリング
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    void ForwardRendering(RenderContext& rc);
    /// <summary>
    /// メインレンダリングターゲットを初期化
    /// </summary>
    void InitMainRenderTarget();
    /// <summary>
    /// メインレンダリングターゲットののスナップショットを取るための
    /// レンダリングターゲットを初期化
    /// </summary>
    void InitMainRTSnapshotRenderTarget();
    /// <summary>
    /// メインレンダリングターゲットのカラーバッファの内容を
    /// フレームバッファにコピーするためのスプライトを初期化する
    /// </summary>
    void InitCopyMainRenderTargetToFrameBufferSprite();
    /// <summary>
    /// ZPrepass用のレンダリングターゲットを初期化
    /// </summary>
    void InitZPrepassRenderTarget();
    /// <summary>
    /// メインレンダリングターゲットのスナップショットを撮影
    /// </summary>
    void SnapshotMainRenderTarget(RenderContext& rc, EnMainRTSnapshot enSnapshot);
    /// <summary>
    /// シャドウマップへの描画処理を初期化
    /// </summary>
    void InitShadowMapRender();
private:
    // GBufferの定義
    enum EnGBuffer
    {
        enGBufferAlbedo,        // アルベド
        enGBufferNormal,        // 法線
        enGBufferWorldPos,      // ワールド座標
        enGBufferMetaricSmooth, // メタリックとスムース。メタリックがr、スムースがa。gbは未使用。
        enGBUfferShadowParam,   // 影パラメータ
        enGBufferNum,           // G-Bufferの数
    };

    // ディファードライティング用の定数バッファ
    struct SDeferredLightingCB
    {
        Light m_light;      // ライト
        float pad;          // パディング
        Matrix mlvp[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
    };

    ShadowMapRender m_shadowMapRenders[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT];   // シャドウマップへの描画処理
    SDeferredLightingCB m_deferredLightingCB;                       // ディファードライティング用の定数バッファ
    Sprite m_copyMainRtToFrameBufferSprite;                         // メインレンダリングターゲットをフレームバッファにコピーするためのスプライト
    Sprite m_diferredLightingSprite;                                // ディファードライティングを行うためのスプライト
    RenderTarget m_zprepassRenderTarget;                            // ZPrepass描画用のレンダリングターゲット
    RenderTarget m_mainRenderTarget;                                // メインレンダリングターゲット
    RenderTarget m_mainRTSnapshots[(int)EnMainRTSnapshot::enNum];   // メインレンダリングターゲットのスナップショット
    RenderTarget m_gBuffer[enGBufferNum];                           // G-Buffer
    PostEffect m_postEffect;                                        // ポストエフェクト
    /*std::vector< Model* > m_zprepassModels;                         // ZPrepassの描画パスで描画されるモデルのリスト
    std::vector< Model* > m_renderToGBufferModels;                  // Gバッファへの描画パスで描画するモデルのリスト
    std::vector< Model* > m_forwardRenderModels;                    // フォワードレンダリングの描画パスで描画されるモデルのリスト
    std::vector< IRender* > m_render2DObject;                       // 2D描画オブジェクトのリスト。*/
    std::vector< IRenderer* > m_renderObjects;                        // 描画オブジェクトのリスト。
};

extern RenderingEngine* g_renderingEngine;