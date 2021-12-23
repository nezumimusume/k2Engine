///////////////////////////////////////
// ZPrepass
///////////////////////////////////////

#include "ModelVSCommon.h"

///////////////////////////////////////
// 構造体。
///////////////////////////////////////


// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;       //座標。
    float3 depth : TEXCOORD0;       //深度値。xにはプロジェクション空間、yにはカメラ空間での正規化されたZ値、zにはカメラ空間でのZ値
};


///////////////////////////////////////
// 関数
///////////////////////////////////////

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal)
{
    SPSIn psIn;
    

    psIn.pos = mul(mWorldLocal, vsIn.pos);  // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos);        // ワールド座標系からカメラ座標系に変換
    psIn.depth.z = psIn.pos.z;
    psIn.pos = mul(mProj, psIn.pos);        // カメラ座標系からスクリーン座標系に変換
    psIn.depth.x = psIn.pos.z / psIn.pos.w;
    psIn.depth.y = saturate( psIn.pos.w / 1000.0f );
    
    return psIn;
}
/// <summary>
/// スキンなしメッシュ用の頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn, mWorld);
}
/// <summary>
/// スキンなしメッシュ用の頂点シェーダーのエントリー関数(インスタンシング描画用)。
/// </summary>
SPSIn VSMainInstancing(SVSIn vsIn, uint instanceID : SV_InstanceID)
{
	return VSMainCore(vsIn, g_worldMatrixArray[instanceID]);
}
/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMainSkin( SVSIn vsIn ) 
{
	return VSMainCore(vsIn, CalcSkinMatrix(vsIn));
}
// モデル用のピクセルシェーダーのエントリーポイント
float4 PSMain(SPSIn psIn) : SV_Target0
{
    return float4( psIn.pos.z, psIn.depth.y, psIn.depth.z, 1.0f );
}
SPSIn VSMainSkinInstancing( SVSIn vsIn, uint instanceID : SV_InstanceID )
{
    float4x4 mWorldLocal = CalcSkinMatrix(vsIn);
    mWorldLocal = mul( g_worldMatrixArray[instanceID], mWorldLocal );
    return VSMainCore(vsIn, mWorldLocal);
}

