///////////////////////////////////////
// ボリュームライトマップ書き込み用のシェーダー。
///////////////////////////////////////

#include "ModelVSCommon.h"



// シーンの深度テクスチャ。
Texture2D<float4> g_sceneDepthTexture : register(t10);

sampler Sampler : register(s0);

///////////////////////////////////////
// 定数バッファ
///////////////////////////////////////
// スポットライト情報
cbuffer SpotLightInfoCb : register(b1)
{
	int no;		// スポットライトの番号。
};

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
/// 前面描画用のピクセルシェーダーエントリー関数。
/// </summary>
float4 PSMainFront(SPSIn psIn) : SV_Target0
{
	// ボリュームライトマップに書き込む深度値を計算する。
	float z = CalcDrawDepth(psIn);
	return float4( z, z, z, z);
}

// ボリュームライトマップ(前面)
Texture2D<float4> g_volumeLightMapFront : register(t10);
// ボリュームライトマップ(背面)
Texture2D<float4> g_volumeLightMapBack : register(t11);


/// <summary>
/// 背面描画用のピクセルシェーダーエントリー関数。
/// </summary>
float4 PSMainBack(SPSIn psIn) : SV_Target0
{
	// ボリュームライトマップに書き込む深度値を計算する。
	float z = CalcDrawDepth(psIn);
	// スポットライトの番号をg成分に書き込む。
	// 整数値を100倍+10で書き込む。ディファードライティング側では
	// 100で除算して、スポットライトの番号に復元する。
	return float4( z, no * 100.0f + 10.0f, z, z);
}

// todo ここら辺後でヘッダーファイルにまとめましょう。
static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_SHADOW_MAP = 3;        // シャドウマップの枚数。
static const int MAX_POINT_LIGHT = 256;     // ポイントライトの最大数。
static const int MAX_SPOT_LIGHT = 256;      // スポットライトの最大数。



///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////

// ディレクションライト構造体。
struct DirectionalLight
{
    float3 direction;   // ライトの方向
    int castShadow;     // 影をキャストする？
    float4 color;       // ライトの色
};
// ポイントライト
struct PointLight
{
    float3 position;        // 座標
    float3 positionInView;  // カメラ空間での座標
    float3 color;           // カラー
    float3 attn;            // 減衰パラメータ。
};
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

// ライト用の定数バッファー
cbuffer LightCb : register(b1)
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    PointLight pointLight[MAX_POINT_LIGHT];
    SpotLight spotLight[MAX_SPOT_LIGHT];
    float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
    float3 eyePos;          // カメラの視点
    int numPointLight;      // ポイントライトの数。    
    float3 ambientLight;    // 環境光
    int numSpotLight;       // スポットライトの数。
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
    float iblLuminance;     // IBLの明るさ。
    int isIBL;              // IBLを行う。
};
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
/// <summary>
/// 最終描画用のピクセルシェーダーエントリー関数。
/// </summary>
float4 PSMainFinal(SPSIn In) : SV_Target0
{
	float3 lig = 0;
	float2 uv = In.posInProj.xy / In.posInProj.w;
	uv = uv * float2( 0.5f, -0.5f ) + float2( 0.5f, 0.5f );
	float4 albedoColor = 1.0f;
	float volumeFrontZ = g_volumeLightMapFront.Sample(Sampler, uv).r;
    float2 volumeBackZ_No = g_volumeLightMapBack.Sample(Sampler, uv).rg;
    float volumeBackZ = volumeBackZ_No.r;
	
    // このピクセルに影響を与えているスポットライトの番号を復元。
    int spotLightNo = (int)(volumeBackZ_No.g / 100.0f);
  
    float3 volumePosBack = CalcWorldPosFromUVZ( uv, volumeBackZ, mViewProjInv);
    float3 volumePosFront = CalcWorldPosFromUVZ( uv, volumeFrontZ, mViewProjInv);
    
    
    SpotLight spLig = spotLight[spotLightNo];

    float t0 = dot( spLig.direction, volumePosFront - spLig.position);
    float t1 = dot( spLig.direction, volumePosBack - spLig.position);
    float t = t0 / (t0 + t1);
    float3 volumeCenterPos = lerp( volumePosFront, volumePosBack, t);
    float volume = length(volumePosBack - volumePosFront);
    if( volume > 0.001f){
        // 距離による減衰。
        float3 ligDir = (volumeCenterPos - spLig.position);
        float distance = length(ligDir);
        ligDir = normalize(ligDir);
        float affect = pow( 1.0f - min(1.0f, distance / spLig.attn.x), spLig.attn.y);
        
        // 続いて角度による減衰を計算する。
        
        // 角度に比例して小さくなっていく影響率を計算する
        float angle = dot(ligDir, spLig.direction);
        // dot()で求めた値をacos()に渡して角度を求める
        angle = abs(acos(angle));
        float angleAffect = max( 0.0f, 1.0f - 1.0f / spLig.attn.z * angle );
        affect *= pow( angleAffect, spLig.attn.w);

        // ボリュームライトの中央地点の光の量を計算する。
        lig += albedoColor * spLig.color * affect * step( volumeFrontZ, albedoColor.w ) * log(volume) * 0.1f;
    }
	return float4( lig, 1.0f);
}
