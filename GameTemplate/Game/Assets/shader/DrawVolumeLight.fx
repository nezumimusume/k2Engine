///////////////////////////////////////
// ボリュームライト描画シェーダー。
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
/// 書き込む深度値を計算する。
/// </summary>
float CalcDrawDepth(SPSIn psIn)
{
	float2 uv = psIn.posInProj.xy / psIn.posInProj.w;
	uv = uv * float2( 0.5f, -0.5f ) + float2( 0.5f, 0.5f );
	float sceneDepth = g_sceneDepthTexture.Sample(Sampler, uv).r;
	float z = min( sceneDepth, psIn.pos.z);
	return z;
}
/// <summary>
/// 前面/背面描画用のピクセルシェーダーエントリー関数。
/// </summary>
float4 PSMainFrontBack(SPSIn psIn) : SV_Target0
{
	// ボリュームライトマップに書き込む深度値を計算する。
	float z = CalcDrawDepth(psIn);
	return float4( z, z, z, z);
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
	screenPos.z = zInScreen;
	
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////


// スポットライト
struct SpotLight
{
    float3 position;        // 座標
    int isUse;              // 使用中フラグ
    float3 positionInView;  // カメラ空間での座標
    int no;                 // ライトの番号。
    float3 color;           // カラー
    float3 direction;       // 射出方向。
    float4 attn;            // 減衰パラメータ。
    float3 directionInView; // カメラ空間での射出方向。
};

///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////

cbuffer cb : register(b0){
	float4x4 mvp;		// モデルビュープロジェクション行列
	float4 mulColor;	// 乗算カラー
};

// ライト用の定数バッファー
cbuffer finalCb : register(b1){
    SpotLight spotLight;    // スポットライト。
    float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
    
};


struct VSFinalInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct PSFinalInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> albedoTexture : register(t0);	// アルベドカラー
Texture2D<float4> g_volumeLightMapFront : register(t1);
Texture2D<float4> g_volumeLightMapBack : register(t2);

PSFinalInput VSFinal(VSFinalInput In) 
{
	PSFinalInput psIn;
	psIn.pos = mul( mvp, In.pos );
	psIn.uv = In.uv;
	return psIn;
}
float4 PSFinal( PSFinalInput In ) : SV_Target0
{
	float3 lig = 0;
	float2 uv = In.uv;
	
	float volumeFrontZ = g_volumeLightMapFront.Sample(Sampler, uv).r;
    float2 volumeBackZ_No = g_volumeLightMapBack.Sample(Sampler, uv).rg;
    float volumeBackZ = volumeBackZ_No.r;
  
    float3 volumePosBack = CalcWorldPosFromUVZ( uv, volumeBackZ, mViewProjInv);
    float3 volumePosFront = CalcWorldPosFromUVZ( uv, volumeFrontZ, mViewProjInv);
    

    float t0 = dot( spotLight.direction, volumePosFront - spotLight.position);
    float t1 = dot( spotLight.direction, volumePosBack - spotLight.position);
    float t = t0 / (t0 + t1);
    float3 volumeCenterPos = lerp( volumePosFront, volumePosBack, t);
    float volume = length(volumePosBack - volumePosFront);

    // ボリュームがない箇所はピクセルキル。
    clip( volume - 0.001f);
    
    float4 albedoColor = albedoTexture.Sample(Sampler, uv);
    
    // 距離による減衰。
    float3 ligDir = (volumeCenterPos - spotLight.position);
    float distance = length(ligDir);
    ligDir = normalize(ligDir);
    float affect = pow( 1.0f - min(1.0f, distance / spotLight.attn.x), spotLight.attn.y);
    
    // 続いて角度による減衰を計算する。
    
    // 角度に比例して小さくなっていく影響率を計算する
    float angle = dot(ligDir, spotLight.direction);
    // dot()で求めた値をacos()に渡して角度を求める
    angle = abs(acos(angle));
    float angleAffect = max( 0.0f, 1.0f - 1.0f / spotLight.attn.z * angle );
    affect *= pow( angleAffect, spotLight.attn.w);

    // ボリュームライトの中央地点の光の量を計算する。
    lig = albedoColor * spotLight.color * affect * step( volumeFrontZ, albedoColor.w ) * max( 0.0f, log(volume) ) * 0.1f;
    
	return float4( lig, 1.0f);
}

float4 PSDepth(SPSIn psIn) : SV_Target0
{
	// 深度値を書き込むだけなので、透明なピクセルを描画する。
	return float4( 0.0f, 0.0f, 0.0f, 0.0f);
}