#pragma once

namespace nsK2Engine {

    /// <summary>
    /// カスケードシャドウマップ法で使用される行列クラス
    /// </summary>
    class CascadeShadowMapMatrix : public Noncopyable
    {
    public:
        /// <summary>
        /// ライトビュープロジェクションクロップ行列を計算する
        /// </summary>
        /// <param name="lightDirection">ライトの方向</param>
        void CalcLightViewProjectionCropMatrix(
            Vector3 lightDirection,
            float cascadeAreaRateTbl[NUM_SHADOW_MAP],
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition
        );
        /// <summary>
        /// 計算されたライトビュープロジェクションクロップ行列を取得
        /// </summary>
        /// <param name="shadowMapNo">シャドウマップの番号</param>
        /// <returns></returns>
        const Matrix& GetLightViewProjectionCropMatrix(int shadowMapNo) const
        {
            return m_lvpcMatrix[shadowMapNo];
        }
    private:
        Matrix m_lvpcMatrix[NUM_SHADOW_MAP];	//ライトビュークロップ行列
        float m_near[NUM_SHADOW_MAP];
        float m_far[NUM_SHADOW_MAP];
    };
}