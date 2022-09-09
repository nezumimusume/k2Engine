/*!
 * @brief ブルーム
 */

#include "../util/ColorSpace.h"

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

/*!
 * @brief 頂点シェーダー
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

Texture2D<float4> mainRenderTargetTexture : register(t0);   // メインレンダリングターゲットのテクスチャ
Texture2D<float4> luminanceAvgTexture : register(t1);       // シーンの平均輝度が記憶されているテクスチャ。
sampler Sampler : register(s0);

/////////////////////////////////////////////////////////
// 輝度抽出用
/////////////////////////////////////////////////////////
cbuffer SamplingLuminanceCb : register(b1)
{
    int isEnableTonemap;
    float middleGray;
    float threshold;
};

/*!
 * @brief 輝度抽出用のピクセルシェーダー
 */
float4 PSSamplingLuminance(PSInput In) : SV_Target0
{
    // メインレンダリングターゲットからカラーをサンプリング
    float4 color = mainRenderTargetTexture.Sample(Sampler, In.uv);

    // サンプリングしたカラーの明るさを計算
    // メインシーンの輝度を求める。
    float luminance = luminanceAvgTexture.Sample(Sampler, In.uv);
    float3 hsv = Rgb2Hsv(color);
    if(isEnableTonemap){
        
        hsv.z = ( middleGray / ( max(luminance, 0.001f ))) * hsv.z;
    }
    // clip()関数は引数の値がマイナスになると、以降の処理をスキップする
    // なので、マイナスになるとピクセルカラーは出力されない
    
    clip(hsv.z - threshold );
    hsv.z -= threshold;
    
    if(isEnableTonemap){
        // カラーを元のカラーに戻す。
        hsv.z *= ( max(luminance, 0.001f )) / 0.18f;
    }
    color.xyz = Hsv2Rgb(hsv);
    return color;
}

// step-5 4枚のボケ画像にアクセスするための変数を追加
Texture2D<float4> g_bokeTexture_0 : register(t0);
Texture2D<float4> g_bokeTexture_1 : register(t1);
Texture2D<float4> g_bokeTexture_2 : register(t2);
Texture2D<float4> g_bokeTexture_3 : register(t3);

float4 PSBloomFinal(PSInput In) : SV_Target0
{
    // step-6 ボケ画像をサンプリングして、平均をとって出力する
    float4 combineColor = g_bokeTexture_0.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_1.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_2.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_3.Sample(Sampler, In.uv);
    combineColor /= 4.0f;
    combineColor.a = 1.0f;
    return combineColor;
}
