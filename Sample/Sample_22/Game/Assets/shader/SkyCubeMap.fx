///////////////////////////////////////
// 3Dモデル用のGBufferへの描画シェーダー。
///////////////////////////////////////

#include "ModelVSCommon.h"

///////////////////////////////////////
// 構造体。
///////////////////////////////////////

// ピクセルシェーダーへの入力
struct SPSIn
{
	float4 pos : SV_POSITION;       //座標。
	float3 normal : NORMAL;         //法線。
	float3 tangent  : TANGENT;      //接ベクトル。
	float3 biNormal : BINORMAL;     //従ベクトル。
	float2 uv : TEXCOORD0;          //UV座標。
	float3 worldPos : TEXCOORD1;    // ワールド座標
};

///////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);      //アルベドマップ
Texture2D<float4> g_normal : register(t1);      //法線マップ
Texture2D<float4> g_spacular : register(t2);    //スペキュラマップ
TextureCube<float4> g_skyCubeMap : register(t10);

///////////////////////////////////////
// サンプラーステート
///////////////////////////////////////
sampler g_sampler : register(s0);


////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

// 法線マップから法線を取得。
float3 GetNormalFromNormalMap(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
	float3 binSpaceNormal = g_normal.SampleLevel(g_sampler, uv, 0.0f).xyz;
	binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

	float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;

	return newNormal;
}

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;
	float4x4 m;
	if (hasSkin) {
		m = CalcSkinMatrix(vsIn);
	}
	else {
		m = mWorld;
	}

	psIn.pos = mul(m, vsIn.pos); // モデルの頂点をワールド座標系に変換
	// 頂点シェーダーからワールド座標を出力
	psIn.worldPos = psIn.pos;

	psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
	psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
	psIn.normal = normalize(mul(m, vsIn.normal));
	psIn.tangent = normalize(mul(m, vsIn.tangent));
	psIn.biNormal = normalize(mul(m, vsIn.biNormal));
	psIn.uv = vsIn.uv;

	return psIn;
}
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn, false);
}
SPSIn VSMainSkin(SVSIn vsIn)
{
	return VSMainCore(vsIn, true);
}

/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
	float4 albedoColor;
	float3 normal = normalize(psIn.normal);
	//albedoColor = g_skyCubeMap.Sample(g_sampler, psIn.normal);
	albedoColor = g_skyCubeMap.Sample(g_sampler, normal);
	//albedoColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return albedoColor;
}
