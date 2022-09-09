#pragma once

#include "MyRenderer.h"
#include "graphics/preRender/ShadowMapRender.h"
#include "graphics/postEffect/PostEffect.h"
#include "graphics/light/SceneLight.h"
#include "graphics/preRender/LightCulling.h"
#include "geometry/SceneGeometryData.h"
#include "graphics/light/VolumeLightRender.h"

namespace nsK2Engine {
   
    /// <summary>
    /// レンダリングエンジン。
    /// </summary>
    /// <remark>
    /// K2Engineの１フレームのレンダリングの流れは次のようになっています。
    /// １.ComputeAnimatedVertex
    ///     ワールド空間に変換済みの頂点を計算する。   
    ///     以降のモデル描画パスでは、このパスで計算された頂点を使い回す。
    /// 　　このパスでインスタンシング描画を行うモデルの頂点バッファは１つの大きなバッファにまとめられる。
    ///     例えば、あるモデルをインスタンシング描画で100頂点のモデルを100体描画する設定になっている場合、
    ///     100頂点 × 100の1万頂点を記憶できる頂点バッファが作られる。
    ///     頂点バッファをまとめる理由はDXRの高速化のため。
    ///     DXRではレイトレワールドに登録するオブジェクトの数が多くなると、効率的なBVHが構築できなくなり、
    ///     大きく速度が低下する。このオブジェクトを１つの頂点バッファに纏めることで大幅な高速化が可能となる。
    /// 
    /// 2. シャドウマップの作成
    ///     カスケードシャドウ方を利用して４つのライトからのシャドウマップを作成する。
    ///     そのため、４つのライト　×　３枚のシャドウマップ(近距離用、中距離用、遠距離用)の合計12枚のシャドウマップが作成される。
    /// 
    /// 3. ZPrepass
    ///     ライトカリングのためのZPrepass。
    ///     無駄なピクセル描画を避けるためではなく、深度テクスチャを作成するために存在しているパス。
    ///     後述するG-Bufferの深度テクスチャを使えばいいように感じるが、
    ///     ディファードとフォワードの両方の深度値が書き込まれたテクスチャが必要だったため追加。
    /// 
    /// 4. ライトカリング
    ///     タイルごとのポイントライトとスポットライトの番号リストを作成。
    /// 
    /// 5. G-Bufferへのレンダリング
    ///     G-Bufferはアルベド&深度値、法線、メタリック&シャドウ&Smoothの３枚。
    /// 
    /// 6. レイトレーシングで映り込み画像を作成する。
    ///     NVIDIAのRTXシリーズのGPUが乗っている場合、レイトレを用いて各オブジェクトへの映り込み画像を生成する。
    ///     このパスはNVIDIAのRTXシリーズでしか通常は使えない。
    ///     ただし、k2EngineLow\k2EngineLowPreCompile.hのENABLE_DXR_ON_RTX_ONLYのマクロを無効にすることで
    ///     DXRが利用できる一部のRTX以外のGPU（NVIDIA GTX1060など）でもレイトレを行える。
    ///     ただし、GTXシリーズではパフォーマンスが大きく損なわれるため、あくまでも開発用。
    /// 
    /// 7. ディファードライティング
    ///     ディズニーベースのPBRライティングが行われる。
    /// 
    /// 8. フォワードレンダリング
    ///     半透明オブジェクトや特殊なシェーディングの描画パス。
    /// 
    /// 9. ポストエフェクト
    ///     下記のポストエフェクトが実行される。
    ///     9.1 スクリーンスペースリフレクション(レイトレを行っている場合は処理されない。)
    ///     9.2 シーンの輝度計算(後述のHDRレンダリングのブルームやトーンマップで使用される。)
    ///     9.3 ブルーム
    ///     9.4 トーンマップ
    ///     9.5 Effekseerのエフェクト描画
    ///     9.6 DOF
    ///     9.7 FXAA + ガンマ補正
    /// 
    /// 10. 2D描画
    /// 11. 最終画像をフレームバッファにコピー。
    /// </reramk>

    class RenderingEngine : public Noncopyable
    {

    public:
        // ディファードライティング用の定数バッファ
        struct SDeferredLightingCB
        {
            Light m_light;              // ライト
            Matrix mlvp[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP]; // ライトビュープロジェクション行列。
            float m_iblLuminance;       // IBLの明るさ。
            int m_isIBL;                // IBLを行う。
            int m_isEnableRaytracing;   // レイトレが行われている。
        };
        /// <summary>
        /// レイトレ用のライトデータ。
        /// </summary>
        struct RaytracingLightData {
            DirectionalLight m_directionalLight;  // ディレクショナルライト。
            Vector3 m_ambientLight;               // 環境光。IBLテクスチャが指定されていない場合に利用される。
            float m_iblIntencity;                 // IBL強度。
            int m_enableIBLTexture;               // IBLテクスチャが指定されている。

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
            enComputeAnimatedVertex,    // アニメーション済み頂点計算。
            enRenderToShadowMap,        // シャドウマップへの描画パス
            enZPrepass,                 // ZPrepass
            enRenderToVolumeLightMap,   // ボリュームライトマップへの描画
            enLightCulling,             // ライトカリング。
            enRenderToGBuffer,          // G-Bufferへの描画パス
            enForwardRender,            // フォワードレンダリングの描画パス
            enPostEffect,               // ポストエフェクト
            enRender2D,                 // 2D描画。
        };
        /// <summary>
        /// イベント。
        /// </summary>
        enum EnEvent {
            enEventReInitIBLTexture,    // IBLテクスチャが再初期化された。
        };
        RenderingEngine();
        ~RenderingEngine();
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
        /// レイトレワールドにモデルを追加。
        /// </summary>
        /// <param name="model">追加するモデル。</param>
        void AddModelToRaytracingWorld(Model& model) 
        {
            g_graphicsEngine->RegistModelToRaytracingWorld(model);
        }
        /// <summary>
        /// レイトレワールドの再構築リクエストを送る。
        /// </summary>
        void RequestRebuildRaytracingWorld()
        {
            g_graphicsEngine->RequestRebuildRaytracingWorld();
        }
        /// <summary>
        /// レイトレワールドからモデルを削除。
        /// </summary>
        /// <param name="model">削除するモデル。</param>
        void RemoveModelFromRaytracingWorld(Model& model) 
        {
            g_graphicsEngine->RemoveModelFromRaytracingWorld(model);
            
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
        /// シーンの平均輝度テクスチャを取得。
        /// </summary>
        /// <returns></returns>o
        Texture& GetLuminanceAvgTextureInScene()
        {
            return m_postEffect.GetLuminanceAvgTextureInScene();
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
        /// 影生成のパラメータを設定する
        /// </summary>
        /// <remark>
        /// 影生成はカスケードシャドウ技法で行っています。
        ///  
        /// </renmark>
        /// <param name=""></param>
        /// <param name="middle"></param>
        /// <param name=""></param>
        /// <param name="lightHeght"></param>
        void SetShadowParameter(float near, float middle, float far, float lightHeght);
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
                [&](const SEventListenerData& listenerData) {return listenerData.pListenerObj == pListenerObj; }
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
        /// <summary>
        /// トーンマップを無効にする
        /// </summary>
        void DisableTonemap()
        {
            m_postEffect.DisableTonemap();
        }
        /// <summary>
        /// トーンマップを有効にする。
        /// </summary>
        void EnableTonemap()
        {
            m_postEffect.EnableTonemap();
        }
        /// <summary>
        /// トーンマップが有効か判定する。
        /// </summary>
        /// <returns></returns>
        bool IsEnableTonemap() const
        {
            return m_postEffect.IsEnableTonemap();
        }
        /// <summary>
        /// シーンの中間の明るさを示す明度率を指定する。
        /// </summary>
        /// <remark>
        /// この設定はポストエフェクトのトーンマップが有効なときに利用されます。
        /// 慣習的に0.18が使われている。
        /// https://en.wikipedia.org/wiki/Middle_gray
        /// </remark>
        void SetSceneMiddleGray(float luminance)
        {
            m_postEffect.SetTonemapMiddlegray(luminance);
        }
        /// <summary>
        /// シーンの中間の明るさを示す明度率を取得する。
        /// </summary>
        /// <returns></returns>
        float GetSceneMiddleGray() const
        {
            return m_postEffect.GetTonemapMiddlegray();
        }
        /// <summary>
        /// ブルームが発生する閾値を設定。
        /// </summary>
        /// <param name="value"></param>
        void SetBloomThreshold(float value)
        {
            m_postEffect.SetBloomThreshold(value);
        }
        /// <summary>
        /// ブルームが発生する閾値を取得。
        /// </summary>
        /// <returns></returns>
        float GetBloomThreshold() const
        {
            return m_postEffect.GetBloomThreshold();
        }
        /// <summary>
        /// ボリュームスポットライトをシーンに追加
        /// </summary>
        /// <param name="lig">ライト</param>
        void AddVolumeSpotLight(VolumeLightBase& lig)
        {
            m_volumeLightRender.AddVolumeSpotLight(lig);
        }
        /// <summary>
        /// ボリュームスポットライトをシーンから削除
        /// </summary>
        /// <param name="lig"></param>
        void RemoveVolumeSpotLight(VolumeLightBase& lig)
        {
            m_volumeLightRender.RemoveVolumeSpotLight(lig);
        }
        /// <summary>
        /// ボリュームライトレンダラーを取得。
        /// </summary>
        /// <returns></returns>
        VolumeLightRender& GetVolumeLightRender()
        {
            return m_volumeLightRender;
        }
        /// <summary>
        /// 環境光の計算のためのIBLテクスチャを設定。
        /// </summary>
        /// <remark>
        /// この関数を利用して、IBLテクスチャをセットすると、
        /// 環境光をIBLテクスチャからサンプリングして、それを利用した
        /// ライティングが行われます。
        /// IBLテクスチャを利用した環境光の計算をオフにしたい場合は、DisableIBLForAmbinet()を呼び出して、
        /// IBLを無効にしてください。
        /// </remark>
        /// <param name="textureFilePath">
        /// IBLテクスチャのファイルパス。
        /// キューブマップである必要があります。
        /// </param>
        /// <param name="luminance">
        /// IBLテクスチャの明るさ。
        /// <param>
        void SetAmbientByIBLTexture(const wchar_t* textureFilePath, float luminance)
        {
            ReInitIBL(textureFilePath, luminance);
        }
        /// <summary>
        /// IBL環境光を無効にする。
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            m_sceneLight.DisableIBLTextureForAmbient();
        }
        /// <summary>
        /// 環境光を設定。
        /// </summary>
        /// <param name="ambient"></param>
        void SetAmbient(Vector3 ambient)
        {
            m_sceneLight.SetAmbinet(ambient);
        }
        /// <summary>
        /// レイトレーシングが有効かどうかを判定する。
        /// </summary>
        /// <returns></returns>
        bool IsEnableRaytracing() const
        {
            return m_isEnableRaytracing && g_graphicsEngine->IsPossibleRaytracing();
        }
        /// <summary>
        /// レイトレーシングを有効にします。
        /// この設定はハードウェアレイトレーシングが無効な場合は無視されます。
        /// </summary>
        void EnableRaytracing()
        {
            m_isEnableRaytracing = true && IsEnableRaytracing();
        }
        /// <summary>
        /// レイトレーシングを無効にします。
        /// この設定はハードウェアレイトレーシングが無効な場合は無視されます。
        /// </summary>
        void DisableRaytracing()
        {
            m_isEnableRaytracing = false && IsEnableRaytracing();
        }
        /// <summary>
        /// レイトレ用のライトデータを取得。
        /// </summary>
        /// <returns></returns>
        RaytracingLightData& GetRaytracingLightData()
        {
            return m_raytracingLightData;
        }
    private:
        /// <summary>
        /// イメージベースドライティング(IBL)のためのデータを初期化する。
        /// </summary>
        /// <param name="iblTexFilePath">IBLテクスチャのファイルパス。</param>
        /// <param name="intencity">IBLの強度。</param>
        void InitIBLData(const wchar_t* iblTexFilePath, float intencity);
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
        /// アニメーション済み頂点の計算。
        /// </summary>
        /// <remark>
        /// </remark>
        /// <param name="rc">レンダリングコンテキスト</param>
        void ComputeAnimatedVertex(RenderContext& rc);
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
            float m_intencity = 1.0f;   // 強度。
        };
        
        /// <summary>
        /// GIテクスチャを作るためのブラー処理。
        /// </summary>
        enum EGITextureBlur {
            eGITextureBlur_1024x1024,   // 1024×1024
            eGITextureBlur_512x512,     // 512×512
            eGITextureBlur_256x256,     // 256×256
            eGITextureBlur_128x128,     // 128×128
            eGITextureBlur_Num,     
        };
        RaytracingLightData m_raytracingLightData;                      // レイトレ用のライトデータ。
        LightCulling m_lightCulling;                                    // ライトカリング。 
        ShadowMapRender m_shadowMapRenders[MAX_DIRECTIONAL_LIGHT];      // シャドウマップへの描画処理
        VolumeLightRender m_volumeLightRender;                          // ボリュームライトレンダラー。
        SDeferredLightingCB m_deferredLightingCB;                       // ディファードライティング用の定数バッファ
        Sprite m_copyMainRtToFrameBufferSprite;                         // メインレンダリングターゲットをフレームバッファにコピーするためのスプライト
        Sprite m_diferredLightingSprite;                                // ディファードライティングを行うためのスプライト
        RenderTarget m_zprepassRenderTarget;                            // ZPrepass描画用のレンダリングターゲット
        RenderTarget m_mainRenderTarget;                                // メインレンダリングターゲット
        RenderTarget m_mainRTSnapshots[(int)EnMainRTSnapshot::enNum];   // メインレンダリングターゲットのスナップショット
        RenderTarget m_gBuffer[enGBufferNum];                           // G-Buffer
        PostEffect m_postEffect;                                        // ポストエフェクト
        RWStructuredBuffer m_pointLightNoListInTileUAV;                 // タイルごとのポイントライトのリストのUAV。
        RWStructuredBuffer m_spotLightNoListInTileUAV;                  // タイルごとのスポットライトのリストのUAV。
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
        bool m_isEnableRaytracing = true;                               // レイトレーシングが有効？
        GaussianBlur m_giTextureBlur[eGITextureBlur_Num];                                // GIテクスチャにブラーをかける処理。
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