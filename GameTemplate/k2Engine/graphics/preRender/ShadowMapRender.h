#pragma once

#include "CascadeShadowMapMatrix.h"


namespace nsK2Engine {
    class IRenderer;
    /// <summary>
    /// シャドウマップへの描画処理
    /// </summary>
    class ShadowMapRender : public Noncopyable
    {
    public:
        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="isSoftShadow">
        /// trueの場合、シャドウマップ法による、影生成がソフトシャドウになります。
        /// ハードシャドウにしたい場合は、falseを指定してください。
        /// </param>
        void Init(bool isSoftShadow);

        /// <summary>
        /// 描画
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        /// <param name="sceneMaxPosition">ゲームシーンの最大座標</param>
        /// <param name="sceneMinPosition">ゲームシーンの最小座標</param>
        void Render(
            RenderContext& rc,
            int ligNo,
            Vector3& lightDirection,
            std::vector< IRenderer* >& renderObjects,
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition
        );
        /// <summary>
        /// シャドウマップを取得
        /// </summary>
        /// <param name="areaNo">エリア番号</param>
        /// <returns></returns>
        Texture& GetShadowMap(int areaNo)
        {
            if (m_isSoftShadow) {
                return m_blur[areaNo].GetBokeTexture();
            }
            return m_shadowMaps[areaNo].GetRenderTargetTexture();
        }
        /// <summary>
        /// ライトビュープロジェクション行列を取得
        /// </summary>
        /// <returns></returns>
        const Matrix& GetLVPMatrix(int areaNo) const
        {
            return m_cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(areaNo);
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
        /// <param name="nearArea">近影エリア率。</param>
        void SetCascadeNearAreaRates(float nearArea, float middleArea, float farArea)
        {
            // 中距離エリアの範囲が近距離エリアの範囲より小さくなっていると
            // 計算上不都合が起きるので、補正。
            middleArea = max(nearArea + 0.01f, middleArea);
            // 遠い距離エリアの範囲が中距離エリアの範囲より小さくなっていると
            // 計算上不都合が起きるので、補正。
            farArea = max(middleArea + 0.01f, farArea);
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_NEAR] = nearArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_MIDDLE] = middleArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_FAR] = farArea;
        }
    private:
        CascadeShadowMapMatrix m_cascadeShadowMapMatrix;    // カスケードシャドウマップの行列を扱うオブジェクト
        RenderTarget m_shadowMaps[NUM_SHADOW_MAP];          // シャドウマップ
        std::vector< IRenderer* > m_renderers;              // シャドウマップへのレンダラーの配列。
        float m_cascadeAreaRateArray[NUM_SHADOW_MAP] = { 0.05f,0.3f, 1.0f };
        GaussianBlur m_blur[NUM_SHADOW_MAP];                // シャドウマップにブラーをかける処理。ソフトシャドウを行う際に使われます。
        bool m_isSoftShadow = false;                        // ソフトシャドウ？
    };
}