/*!
 * @brief	シーンの平均輝度計算用のシェーダー。
 */

#include "../util/ColorSpace.h"

/*!
 *@brief    頂点シェーダーへの入力
 */
struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};
/*!
 *@brief    ピクセルシェーダーへの入力
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief    スプライト描画の共通定数バッファ。
 *@details  Spriteクラス内でb0レジスタに設定されています。
 */
cbuffer cb0 : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};

/*!
 *@brief    平均輝度計算用の定数バッファ。
 */
cbuffer cb1 : register(b1){
	float deltaTime;    // 経過時間。
}

/*!
 * @brief	頂点シェーダー。
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

////////////////////////////////////////////////////////
// 輝度の対数平均を求める。
////////////////////////////////////////////////////////

Texture2D<float4> sceneTexture : register(t0);	//シーンテクスチャ。
sampler Sampler : register(s0);

static const int    MAX_SAMPLES = 16;    // Maximum texture grabs

/*!
 * @brief	定数バッファ。
 */
cbuffer cbCalcLuminanceLog : register(b0){
	float4 g_avSampleOffsets[MAX_SAMPLES];
};
/*!
 *@brief	輝度の対数平均を求める。
 */
float4 PSCalcLuminanceLogAvarage(PSInput In) : SV_Target0
{
	float3 vSample = 0.0f;
    float  fLogLumSum = 0.0f;

    for(int iSample = 0; iSample < 9; iSample++)
    {

        vSample = max( sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy), 0.001f );
        float v = Rgb2V( vSample );
        fLogLumSum += log(v);
    }
    
    fLogLumSum /= 9;

    return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);
}
////////////////////////////////////////////////////////
// 輝度の平均を求める。
////////////////////////////////////////////////////////
/*!
 *@brief	平均輝度計算ピクセルシェーダー。
 */
float4 PSCalcLuminanceAvarage(PSInput In) : SV_Target0
{
	float fResampleSum = 0.0f; 
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy);
    }
    
    // Divide the sum to complete the average
    fResampleSum /= 16;

    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// 指数関数を使用して平均輝度を求める
/////////////////////////////////////////////////////////
/*!
 *@brief	指数関数を使用して平均輝度を求めるピクセルシェーダー。
 */
float4 PSCalcLuminanceExpAvarage( PSInput In ) : SV_Target0
{
	float fResampleSum = 0.0f;
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample]);
    }
    
    // Divide the sum to complete the average, and perform an exp() to complete
    // the average luminance calculation
    fResampleSum = exp(fResampleSum/16);
    
    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// 明暗順応
/////////////////////////////////////////////////////////

Texture2D<float4> lumAvgTexture : register(t0);		                // 現在のフレームの平均輝度。
Texture2D<float4> lumAvgInTonemapLastFrameTexture : register(t1);	// １フレーム前のトーンマップで使用した平均輝度。

/*!
 *@brief	明暗順応のための平均輝度の適合させるピクセルシェーダー。
 */
float4 PSCalcAdaptedLuminance( PSInput In ) : SV_Target0
{
	// 1フレーム前のトーンマップで使用した平均輝度をサンプリング。
	float lumAvgLastFrame = lumAvgInTonemapLastFrameTexture.Sample(Sampler, float2( 0.5f, 0.5f));
    // 現在のフレームの平均輝度をサンプリング。
    float lumAvgCurrentFrame = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    // 線形補間でちょっとずつ平均輝度を変化させていく。
    float newLumAvg = lerp( lumAvgLastFrame, lumAvgCurrentFrame, 0.02f);
    return float4(newLumAvg, newLumAvg, newLumAvg, 1.0f);
}

float4 PSCalcAdaptedLuminanceFirst(PSInput In) : SV_Target0
{
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
	return float4(fAvgLum, fAvgLum, fAvgLum, 1.0f);
}