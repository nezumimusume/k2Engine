/*!
 * @brief	シンプルなモデルシェーダー。
 */
#include "ModelVSCommon.h"

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////

// ピクセルシェーダーへの入力
struct SPSIn
{
	float4 pos : SV_POSITION; //座標。
	float3 normal : NORMAL; //法線。
	float3 tangent : TANGENT; //接ベクトル。
	float3 biNormal : BINORMAL; //従ベクトル。
	float2 uv : TEXCOORD0; //UV座標。
	float3 worldPos : TEXCOORD1; // ワールド座標
};

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
///////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////
Texture2D<float4> g_albedo : register(t0); //アルベドマップ
Texture2D<float4> g_normal : register(t1); //法線マップ
Texture2D<float4> g_spacular : register(t2); //スペキュラマップ

sampler g_sampler : register(s0);	//サンプラステート。

////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal)
{
	SPSIn psIn;
    
	psIn.pos = mul(mWorldLocal, vsIn.pos); // モデルの頂点をワールド座標系に変換
    // 頂点シェーダーからワールド座標を出力
	psIn.worldPos = psIn.pos;

	psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
	psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
	psIn.normal = normalize(mul(mWorldLocal, vsIn.normal));
	psIn.tangent = normalize(mul(mWorldLocal, vsIn.tangent));
	psIn.biNormal = normalize(mul(mWorldLocal, vsIn.biNormal));
	psIn.uv = vsIn.uv;
	
	return psIn;
}
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn, mWorld);
}
SPSIn VSMainSkin(SVSIn vsIn)
{
	return VSMainCore(vsIn, CalcSkinMatrix(vsIn));
}
SPSIn VSMainInstancing(SVSIn vsIn, uint instanceID : SV_InstanceID)
{
	return VSMainCore(vsIn, g_worldMatrixArray[instanceID]);
}
SPSIn VSMainSkinInstancing(SVSIn vsIn, uint instanceID : SV_InstanceID)
{
	float4x4 mWorldLocal = CalcSkinMatrix(vsIn);
	mWorldLocal = mul(g_worldMatrixArray[instanceID], mWorldLocal);
	return VSMainCore(vsIn, mWorldLocal);
}

/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain( SPSIn psIn ) : SV_Target0
{
	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);
	return albedoColor;
}
