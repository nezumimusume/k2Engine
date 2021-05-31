/*!
 * @brief シャドウマップ描画用のシェーダー
 */

#include "ModelVSCommon.h"

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;   // スクリーン空間でのピクセルの座標
};

///////////////////////////////////////////////////
// グローバル変数
///////////////////////////////////////////////////

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
    SPSIn psIn;
    float4x4 m;
    if( hasSkin ){
        m = CalcSkinMatrix(vsIn);
    }else{
        m = mWorld;
    }

    psIn.pos = mul(m, vsIn.pos);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    return psIn;
}
/// <summary>
/// 頂点シェーダー
/// <summary>
SPSIn VSMain(SVSIn vsIn)
{
    // シャドウマップ描画用の頂点シェーダーを実装
    return VSMainCore(vsIn, false);
}
/// <summary>
/// 頂点シェーダー
/// <summary>
SPSIn VSSkinMain(SVSIn vsIn)
{
    // シャドウマップ描画用の頂点シェーダーを実装
    return VSMainCore(vsIn, true);
}
/// <summary>
/// シャドウマップ描画用のピクセルシェーダー
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
    return float4(psIn.pos.z, psIn.pos.z, psIn.pos.z, 1.0f);
}
