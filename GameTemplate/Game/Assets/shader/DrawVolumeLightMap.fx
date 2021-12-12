///////////////////////////////////////
// ボリュームライトマップ書き込み用のシェーダー。
///////////////////////////////////////

#include "ModelVSCommon.h"

///////////////////////////////////////
// 構造体。
///////////////////////////////////////

// ピクセルシェーダーへの入力
struct SPSIn
{
	float4 pos : SV_POSITION;       //座標。
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
	
	return psIn;
}


/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
	return float4( 1.0f, 0.0f, 0.0f, 1.0f);
}
