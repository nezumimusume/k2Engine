/*!
 * @brief	シンプルなモデルシェーダー。
 */
#include "ModelVSCommon.h"

///////////////////////////////////////
// 定数
///////////////////////////////////////
static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_SHADOW_MAP = 3;        // シャドウマップの枚数。

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
Texture2D<float4> albedoTexture : register(t0);     // アルベド
Texture2D<float4> normalTexture : register(t1);     // 法線
Texture2D<float4> metallicShadowSmoothTexture : register(t2);   // メタリック、シャドウ、スムーステクスチャ。rに金属度、gに影パラメータ、aに滑らかさ。
TextureCube<float4> g_skyCubeMap : register(t11);
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t12);  //シャドウマップ。

#include "PBRLighting.h"

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
float4 PSMainCore( SPSIn In, uniform int isSoftShadow ) 
{
	//G-Bufferの内容を使ってライティング
    //アルベドカラーをサンプリング。
    float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);
    //法線をサンプリング。
    float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
    //ワールド座標をサンプリング。
    float3 worldPos = In.worldPos;
    //スペキュラカラーをサンプリング。
    float3 specColor = albedoColor.xyz;
    //金属度をサンプリング。
    float metaric = metallicShadowSmoothTexture.SampleLevel(Sampler, In.uv, 0).r;
    //スムース
    float smooth = metallicShadowSmoothTexture.SampleLevel(Sampler, In.uv, 0).a;

    //影生成用のパラメータ。
    float shadowParam = 1.0f;
    
    float2 viewportPos = In.pos.xy;

	 // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(eyePos - worldPos);

    float3 lig = 0;
    
    for(int ligNo = 0; ligNo < NUM_DIRECTIONAL_LIGHT; ligNo++)
    {
        // 影の落ち具合を計算する。
        float shadow = 0.0f;
        if( directionalLight[ligNo].castShadow == 1){
            //影を生成するなら。
            shadow = CalcShadowRate( ligNo, worldPos, isSoftShadow ) * shadowParam;
        }
        
        lig += CalcLighting(
            directionalLight[ligNo].direction,
            directionalLight[ligNo].color,
            normal,
            toEye,
            albedoColor,
            metaric,
            smooth,
            specColor
        ) * ( 1.0f - shadow );
    }
	
	 if (isIBL == 1) {
        // 視線からの反射ベクトルを求める。
        float3 v = reflect(toEye * -1.0f, normal);
        int level = lerp(0, 12, 1 - smooth);
        lig += albedoColor * g_skyCubeMap.SampleLevel(Sampler, v, level) * iblLuminance;
    }
    else {
        // 環境光による底上げ
        lig += ambientLight * albedoColor;
    }
   
	float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return float4(finalColor.xyz, albedoColor.a);
}
float4 PSMainSoftShadow(SPSIn In) : SV_Target0
{
    return PSMainCore( In, true);
}
//ハードシャドウを行うピクセルシェーダー。
float4 PSMainHardShadow(SPSIn In) : SV_Target0
{
    return PSMainCore( In, false);
} 