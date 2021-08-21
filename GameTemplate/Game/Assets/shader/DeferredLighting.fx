///////////////////////////////////////
// PBRベースのディファードライティング
///////////////////////////////////////

#include "PBRLighting.h"


///////////////////////////////////////
// 構造体。
///////////////////////////////////////

//頂点シェーダーへの入力構造体。
struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};
//ピクセルシェーダーへの入力構造体。
struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////
//共通定数バッファ
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
    float4 screenParam;
};

///////////////////////////////////////
// テクスチャ
///////////////////////////////////////
Texture2D<float4> albedoTexture : register(t0);     // アルベド
Texture2D<float4> normalTexture : register(t1);     // 法線
Texture2D<float4> metallicShadowSmoothTexture : register(t2);   // メタリック、シャドウ、スムーステクスチャ。rに金属度、gに影パラメータ、aに滑らかさ。

///////////////////////////////////////
// 関数
///////////////////////////////////////

//頂点シェーダー。
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}


/*!
 * @brief	UV座標と深度値からワールド座標を計算する。
 *@param[in]	uv				uv座標
 *@param[in]	zInScreen		スクリーン座標系の深度値
 *@param[in]	mViewProjInv	ビュープロジェクション行列の逆行列。
 */
float3 CalcWorldPosFromUVZ( float2 uv, float zInScreen, float4x4 mViewProjInv )
{
	float3 screenPos;
	screenPos.xy = (uv * float2(2.0f, -2.0f)) + float2( -1.0f, 1.0f);
	screenPos.z = zInScreen;//depthMap.Sample(Sampler, uv).r;
	
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
//ピクセルシェーダーのコア。
float4 PSMainCore(PSInput In, uniform int isSoftShadow) : SV_Target0
{
    //G-Bufferの内容を使ってライティング
    //アルベドカラーをサンプリング。
    float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);
    //法線をサンプリング。
    float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
    //ワールド座標をサンプリング。
    float3 worldPos = CalcWorldPosFromUVZ(In.uv, albedoColor.w, mViewProjInv);
    //スペキュラカラーをサンプリング。
    float3 specColor = albedoColor.xyz;
    //金属度をサンプリング。
    float metaric = metallicShadowSmoothTexture.SampleLevel(Sampler, In.uv, 0).r;
    //スムース
    float smooth = metallicShadowSmoothTexture.SampleLevel(Sampler, In.uv, 0).a;

    //影生成用のパラメータ。
    float shadowParam = metallicShadowSmoothTexture.Sample(Sampler, In.uv).g;

    float3 lig = CalcPBRLighting(
        albedoColor,
        normal,
        worldPos,
        specColor,
        metaric,
        smooth,
        shadowParam,
        isSoftShadow,
        In.pos.xy,
        screenParam
    );
   
    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return finalColor;
}
float4 PSMainSoftShadow(PSInput In) : SV_Target0
{
    return PSMainCore( In, true);
}
//ハードシャドウを行うピクセルシェーダー。
float4 PSMainHardShadow(PSInput In) : SV_Target0
{
    return PSMainCore( In, false);
} 