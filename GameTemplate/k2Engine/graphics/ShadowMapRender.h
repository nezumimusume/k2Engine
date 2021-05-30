#pragma once

#include "CascadeShadowMapMatrix.h"


class IRenderer;

/// <summary>
/// シャドウマップへの描画処理
/// </summary>
class ShadowMapRender
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Init();
    
    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="rc">レンダリングコンテキスト</param>
    /// <param name="lightDirection">ライトの方向</param>
    void Render(
        RenderContext& rc, 
        int ligNo, 
        Vector3& lightDirection,
        std::vector< IRenderer* >& renderObjects
    );
    /// <summary>
    /// シャドウマップを取得
    /// </summary>
    /// <param name="areaNo">エリア番号</param>
    /// <returns></returns>
    Texture& GetShadowMap(int areaNo)
    {
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
private:
    CascadeShadowMapMatrix m_cascadeShadowMapMatrix;    //カスケードシャドウマップの行列を扱うオブジェクト
    RenderTarget m_shadowMaps[NUM_SHADOW_MAP];          //シャドウマップ
    std::vector< IRenderer* > m_renderers;              //シャドウマップへのレンダラーの配列。
};
   