#pragma once

#include "MyRenderer.h"
#include "graphics/preRender/ShadowMapRender.h"
#include "graphics/postEffect/PostEffect.h"
#include "SceneLight.h"
#include "graphics/preRender/LightCulling.h"
#include "geometry/SceneGeometryData.h"

namespace nsK2Engine {
   
    /// <summary>
    /// レンダリングエンジン。
    /// </summary>
    /// <remark>
    /// このクラスはシングルトンパターンで設計されています。
    /// インスタンスの生成はCreateInstance()、
    /// インスタンスの破棄はDeleteInstance()、
    /// インスタンスの取得はCreateInstance()を呼び出してください。
    /// シングルトンパターンは下記の二つの機能を提供するパターンです。
    /// 1.　グローバルなアクセスポイント
    ///		->グローバル変数の機能
    /// 2　インスタンスの数を一つに制限する機能。
    /// </reramk>

    class RenderingEngine : public Noncopyable
    {
    
    public:
        // ディファードライティング用の定数バッファ
        struct SDeferredLightingCB
        {
            Light m_light;      // ライト
            int m_isIBL;        // IBLを行う。
            Matrix mlvp[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
            float m_iblLuminance;   // IBLの明るさ。
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
            enRenderToShadowMap,    // シャドウマップへの描画パス
            enZPrepass,             // ZPrepass
            enLightCulling,         // ライトカリング。
            enRenderToGBuffer,      // G-Bufferへの描画パス
            enForwardRender,        // フォワードレンダリングの描画パス
            enPostEffect,           // ポストエフェクト
            enRender2D,             // 2D描画。
        };
        /// <summary>
        /// イベント。
        /// </summary>
        enum EnEvent {
            enEventReInitIBLTexture,    // IBLテクスチャが再初期化された。
        };
        /// <summary>
        /// レンダリングパイプラインを初期化
        /// </summary>
        /// <param name="isSoftShadow">
        /// trueの場合、シャドウマップ法による、影生成がソフトシャドウになります。
        /// ハードシャドウにしたい場合は、falseを指定してください。
        /// </param>
        void Init(bool isSoftShadow);

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
            return m_gBuffer[enGBufferAlbedoDepth].GetRenderTargetTexture();
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
            m_sceneLight.SetDirectionLight(lightNo, direction, color);
        }
        void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
        {
            rc.SetRenderTarget(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget.GetDSVCpuDescriptorHandle());
        }
        /// <summary>
        /// ソフトシャドウを行うか判定。
        /// </summary>
        /// <returns></returns>
        bool IsSoftShadow() const
        {
            return m_isSoftShadow;
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
        /// <summary>
        /// ビューカリングのためのビュープロジェクション行列を取得。
        /// </summary>
        /// <returns></returns>
        const Matrix& GetViewProjectionMatrixForViewCulling() const
        {
            return m_viewProjMatrixForViewCulling;
        }
        /// <summary>
        /// シーンが切り替わったことを通知する。
        /// </summary>
        /// <remark>
        /// シーンの連続性を利用したグラフィック処理のための関数です。
        /// 明暗順応、TAA等の処理は、シーンが切り替わったときにこの関数を呼び出さないと
        /// 数フレームの間、表現が多少おかしくなります。
        /// </remark>
        /// <param name="changeSceneTime">シーン切り替えにかかる時間。</param>
        void NotifyChangeScene(float changeSceneTime)
        {
            m_postEffect.NotifyChangeScene(changeSceneTime);
        }
        /// <summary>
        /// 幾何学データを登録
        /// </summary>
        /// <param name="geomData">幾何学データ</param>
        void RegisterGeometryData(GemometryData* geomData)
        {
            m_sceneGeometryData.RegisterGeometryData(geomData);
        }
        /// <summary>
        /// 幾何学データの登録解除。
        /// </summary>
        /// <param name="geomData"></param>
        void UnregisterGeometryData(GemometryData* geomData)
        {
            m_sceneGeometryData.UnregisterGeometryData(geomData);
        }
        /// <summary>
        /// 更新。
        /// </summary>
        void Update();
        /// <summary>
        /// イベントリスナーを追加。
        /// </summary>
        void AddEventListener(
            void* pListenerObj,
            std::function<void(EnEvent enEvent)> listenerFunc
        )
        {
            m_eventListeners.push_back({ pListenerObj, listenerFunc });
        }
        /// <summary>
        /// イベントリスナーを削除。
        /// </summary>
        void RemoveEventListener(void* pListenerObj)
        {
            
            auto it = std::find_if(
                m_eventListeners.begin(),
                m_eventListeners.end(),
                [&](const SEventListenerData& listenerData){return listenerData.pListenerObj == pListenerObj;}
            );
            if (it != m_eventListeners.end()) {
                m_eventListeners.erase(it);
            }
        }
        /// <summary>
        /// IBLを再初期化。
        /// </summary>
        void ReInitIBL(const wchar_t* iblTexFilePath, float luminance);
        /// <summary>
        /// IBLテクスチャを取得。
        /// </summary>
        /// <returns></returns>
        Texture& GetIBLTexture()
        {
            return m_iblData.m_texture;
        }
        /// <summary>
        /// シャドウマップテクスチャにクエリを行う。
        /// </summary>
        /// <param name="queryFunc">クエリ関数</param>
        void QueryShadowMapTexture(std::function< void(Texture& shadowMap) > queryFunc)
        {
            for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
            {
                for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
                {
                    queryFunc(m_shadowMapRenders[i].GetShadowMap(areaNo));
                }
            }
        }
        SDeferredLightingCB& GetDeferredLightingCB()
        {
            return m_deferredLightingCB;
        }
    private:
        /// <summary>
        /// イメージベースドライティング(IBL)のためのデータを初期化する。
        /// </summary>
        /// <param name="iblTexFilePath">IBLテクスチャのファイルパス。</param>
        /// <param name="luminance">IBLテクスチャの明るさ。</param>
        void InitIBLData(const wchar_t* iblTexFilePath, float luminance);
        /// <summary>
        /// G-Bufferを初期化
        /// </summary>
        void InitGBuffer();
        /// <summary>
        /// ディファードライティングの初期化
        /// </summary>
        void InitDeferredLighting();
        /// <summary>
        /// ディファードライティングで使用するスプライトを初期化。
        /// </summary>
        void InitDefferedLighting_Sprite();
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
        /// <summary>
        /// ビューカリング用のビュープロジェクション行列を計算。
        /// </summary>
        void CalcViewProjectionMatrixForViewCulling();
        /// <summary>
        /// 2D描画用のレンダ―ターゲットを初期化
        /// </summary>
        void Init2DRenderTarget();
    private:
        // GBufferの定義
        enum EnGBuffer
        {
            enGBufferAlbedoDepth,           // アルベドと深度値。αに深度値が記憶されています。
            enGBufferNormal,                // 法線
            enGBufferMetaricShadowSmooth,   // メタリック、影パラメータ、スムース。
                                            // メタリックがr、影パラメータがg、スムースがa。gは未使用。
                                            enGBufferNum,                   // G-Bufferの数
        };

        
        /// <summary>
        /// IBLデータ
        /// </summary>
        struct SIBLData {
            Texture m_texture;          // IBLテクスチャ
            float m_luminance = 1.0f;   // 明るさ。
        };
        LightCulling m_lightCulling;                                    // ライトカリング。 
        ShadowMapRender m_shadowMapRenders[MAX_DIRECTIONAL_LIGHT];      // シャドウマップへの描画処理
        SDeferredLightingCB m_deferredLightingCB;                       // ディファードライティング用の定数バッファ
        Sprite m_copyMainRtToFrameBufferSprite;                         // メインレンダリングターゲットをフレームバッファにコピーするためのスプライト
        Sprite m_diferredLightingSprite;                                // ディファードライティングを行うためのスプライト
        RenderTarget m_zprepassRenderTarget;                            // ZPrepass描画用のレンダリングターゲット
        RenderTarget m_mainRenderTarget;                                // メインレンダリングターゲット
        RenderTarget m_mainRTSnapshots[(int)EnMainRTSnapshot::enNum];   // メインレンダリングターゲットのスナップショット
        RenderTarget m_gBuffer[enGBufferNum];                           // G-Buffer
        PostEffect m_postEffect;                                        // ポストエフェクト
        RWStructuredBuffer m_pointLightNoListInTileUAV;                 // タイルごとのポイントライトのリストのUAV。
        std::vector< IRenderer* > m_renderObjects;                      // 描画オブジェクトのリスト。
        SceneLight m_sceneLight;                                        // シーンライト。
        bool m_isSoftShadow = false;                                    // ソフトシャドウフラグ。
        Matrix m_viewProjMatrixForViewCulling;                          // ビューカリング用のビュープロジェクション行列。
        SceneGeometryData m_sceneGeometryData;                          // シーンのジオメトリ情報。
        static RenderingEngine* m_instance;		                        // 唯一のインスタンスのアドレスを記録する変数。
        RenderTarget m_2DRenderTarget;                                  // 2D描画用のレンダ―ターゲット。
        Sprite m_2DSprite;                                              // 2D合成用のスプライト。
        Sprite m_mainSprite;
        SIBLData m_iblData;                                             // IBLデータ。

        /// <summary>
        /// イベントリスナーのデータ。
        /// </summary>
        struct SEventListenerData {
            void* pListenerObj;     // リスナーオブジェクト
            std::function<void(EnEvent enEvent)> listenerFunc;
        };
        
        std::list< SEventListenerData > m_eventListeners;                // イベントリスナー。
    };    
}