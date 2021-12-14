///////////////////////////////////////
// ボリュームライトマップ書き込み用のシェーダー。
///////////////////////////////////////

#include "ModelVSCommon.h"

// シーンの深度テクスチャ。
Texture2D<float4> g_sceneDepthTexture : register(t10);

sampler Sampler : register(s0);

///////////////////////////////////////
// 構造体。
///////////////////////////////////////

// ピクセルシェーダーへの入力
struct SPSIn
{
	float4 pos : SV_POSITION;       //座標。
	float4 posInProj : TEXCOORD0;
	float3 worldPos : TEXCOORD1;    // ワールド座標
};

SPSIn VSMain(float4 pos : POSITION)
{
	SPSIn psIn;
	psIn.pos = mul(mWorld, pos); // モデルの頂点をワールド座標系に変換
	// 頂点シェーダーからワールド座標を出力
	psIn.worldPos = pos;
	psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
	psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
	psIn.posInProj = psIn.pos;

	return psIn;
}


/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
	float2 uv = psIn.posInProj.xy / psIn.posInProj.w;
	uv = uv * float2( 0.5f, -0.5f ) + float2( 0.5f, 0.5f );
	float sceneDepth = g_sceneDepthTexture.Sample(Sampler, uv).r;
	float z = min( sceneDepth, psIn.pos.z);
	
	return float4( z, z, z, z);
}
