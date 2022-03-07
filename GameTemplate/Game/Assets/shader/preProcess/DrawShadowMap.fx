/*!
 * @brief シャドウマップ描画用のシェーダー
 */

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;   // スクリーン空間でのピクセルの座標
    float2 depth : TEXCOORD1;   // ライト空間での深度情報
};

static const int INFINITY = 40.0f; 

///////////////////////////////////////
// 頂点シェーダーの共通処理をインクルードする。
///////////////////////////////////////

#include "../ModelVSCommon.h"


///////////////////////////////////////////////////
// グローバル変数
///////////////////////////////////////////////////

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal, uniform bool isUsePreComputedVertexBuffer)
{
    SPSIn psIn;

    psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal, isUsePreComputedVertexBuffer);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    return psIn;
}
/// <summary>
/// シャドウマップ描画用のピクセルシェーダー
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
    float depth = psIn.pos.z ;
    float pos = exp(INFINITY * depth);
    return float4(pos, pos*pos, 0.0f, 1.0f);
}
