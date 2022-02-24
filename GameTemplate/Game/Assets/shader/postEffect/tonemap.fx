/*!
 * @brief	トーンマップ。
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
 *@brief    ピクセルシェーダーへの入力。
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief    スプライト描画の共通定数バッファ。
 *@details  Spriteクラス内でb0レジスタに設定されています。
 */
cbuffer SpriteCommonCb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};


/*!
 *@brief    トーンマップ専用の定数バッファ。
 */
cbuffer cb2 : register(b1){
	float middleGray;   
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


Texture2D<float4> sceneTexture : register(t0);	            // シーンテクスチャ。
Texture2D<float4> lumAvgInTonemapTexture : register(t1);    // 輝度テクスチャ。
sampler Sampler : register(s0);

/*!
 *@brief    ACESを使ったトーンマッパー
 */
float ACESFilm(float x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

/*!
 *@brief	平均輝度からトーンマップを行うピクセルシェーダー。
 */
float4 PSFinal( PSInput In) : SV_Target0
{
	// シーンのカラーをサンプリングする。
	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv );
    // シーンのカラーをRGB系からHSV系に変換する。
    float3 hsv = Rgb2Hsv(sceneColor);

	float avgLum = lumAvgInTonemapTexture.Sample(Sampler, float2( 0.5f, 0.5f)).r;
    
    // 平均輝度をmiddleGrayの値にするためのスケール値を求める。
    float k = ( middleGray / ( max(avgLum, 0.001f )));
    // スケール値を使って、輝度をスケールダウン。
    hsv.z *= k;
    
    // ACESトーンマッパーを使って輝度の変化を非線形にする。
    hsv.z = ACESFilm(hsv.z);

    // HSV系からRGB系に戻す。
    sceneColor.xyz = Hsv2Rgb(hsv);
    
	return sceneColor;
}