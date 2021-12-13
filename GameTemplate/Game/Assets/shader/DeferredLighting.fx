///////////////////////////////////////
// PBRベースのディファードライティング
///////////////////////////////////////


///////////////////////////////////////
// 定数
///////////////////////////////////////
static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int NUM_SHADOW_MAP = 3;        // シャドウマップの枚数。

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
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t3);  //シャドウマップ。
TextureCube<float4> g_skyCubeMap : register(t15);
// タイルごとのポイントライトのインデックスのリスト
StructuredBuffer<uint> pointLightListInTile : register(t20);
// タイルごとのスポットライトのインデックスのリスト。
StructuredBuffer<uint> spotLightListInTile : register(t21);
// ボリュームライトマップ(背面)
Texture2D<float4> g_volumeLightMapBack : register(t22);
// ボリュームライトマップ(前面)
Texture2D<float4> g_volumeLightMapFront : register(t23);

#include "PBRLighting.h"

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
/*!
 * @brief	ディレクションライトの反射光を計算
 *@param[in]	normal			サーフェイスの法線。
 *@param[in]	toEye           サーフェイスから視点への方向ベクトル(単位ベクトル)
 *@param[in]	albedoColor     アルベドカラー
 *@param[in]    metaric         メタリック
 *@param[in]    smooth          滑らかさ
 *@param[in]    specColor       スペキュラカラー
 *@param[in]    worldPos        サーフェイスのワールド座標
 *@param[in]    isSoftShadow    ソフトシャドウ？
 *@param[in]    shadowParam     シャドウレシーバーフラグ。
 */
float3 CalcDirectionLight(
    float3 normal, 
    float3 toEye, 
    float4 albedoColor, 
    float metaric, 
    float smooth, 
    float3 specColor,
    float3 worldPos,
    bool isSoftShadow,
    float shadowParam
){
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
    return lig;
}
/*!
 *@brief	ポイントライトの反射光を計算
 *@param[in]	normal			サーフェイスの法線。
 *@param[in]	toEye           サーフェイスから視点への方向ベクトル(単位ベクトル)
 *@param[in]	albedoColor     アルベドカラー
 *@param[in]    metaric         メタリック
 *@param[in]    smooth          滑らかさ
 *@param[in]    specColor       スペキュラカラー
 *@param[in]    worldPos        サーフェイスのワールド座標
 */
float3 CalcPointLight(
    float3 normal, 
    float3 toEye, 
    float4 albedoColor, 
    float metaric, 
    float smooth, 
    float3 specColor,
    float3 worldPos,
    float2 viewportPos
){
    float3 lig = 0;
    
    // スクリーンの左上を(0,0)、右下を(1,1)とする座標系に変換する
    // ビューポート座標系に変換する
    // このピクセルが含まれているタイルの番号を計算する
    // スクリーンをタイルで分割したときのセルのX座標を求める
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;

    // タイルインデックスを計算する
    uint tileIndex = floor(viewportPos.x / TILE_WIDTH) + floor(viewportPos.y / TILE_HEIGHT) * numCellX;

    // 含まれるタイルの影響リストの開始位置と終了位置を計算する
    uint lightStart = tileIndex * numPointLight;
    uint lightEnd = lightStart + numPointLight;
    for (uint lightListIndex = lightStart; lightListIndex < lightEnd; lightListIndex++)
    {
        uint ligNo = pointLightListInTile[lightListIndex];
        if (ligNo == 0xffffffff)
        {
            // このタイルに含まれるポイントライトはもうない
            break;
        }
        
        float3 ligDir = normalize(worldPos - pointLight[ligNo].position);
        // 2. 光源からサーフェイスまでの距離を計算
        float distance = length(worldPos - pointLight[ligNo].position);
        float3 ligColor = pointLight[ligNo].color;
        float3 ptLig = CalcLighting(
            ligDir,
            ligColor,
            normal,
            toEye,
            albedoColor,
            metaric,
            smooth,
            specColor
        );
        // 3. 影響率を計算する。影響率は0.0～1.0の範囲で、
        //     指定した距離（pointsLights[i].range）を超えたら、影響率は0.0になる
        float affect = 1.0f - min(1.0f, distance / pointLight[ligNo].attn.x);
        affect = pow( affect, pointLight[ligNo].attn.y );
        lig += ptLig * affect;
    }
    return lig;
}
/*!
 *@brief	スポットライトの反射光を計算
 *@param[in]	normal			サーフェイスの法線。
 *@param[in]	toEye           サーフェイスから視点への方向ベクトル(単位ベクトル)
 *@param[in]	albedoColor     アルベドカラー
 *@param[in]    metaric         メタリック
 *@param[in]    smooth          滑らかさ
 *@param[in]    specColor       スペキュラカラー
 *@param[in]    worldPos        サーフェイスのワールド座標
 */
float3 CalcSpotLight(
    float3 normal, 
    float3 toEye, 
    float4 albedoColor, 
    float metaric, 
    float smooth, 
    float3 specColor,
    float3 worldPos,
    float2 viewportPos
){
    float3 lig = 0;
    // スクリーンの左上を(0,0)、右下を(1,1)とする座標系に変換する
    // ビューポート座標系に変換する

    // このピクセルが含まれているタイルの番号を計算する
    // スクリーンをタイルで分割したときのセルのX座標を求める
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;

    // タイルインデックスを計算する
    uint tileIndex = floor(viewportPos.x / TILE_WIDTH) + floor(viewportPos.y / TILE_HEIGHT) * numCellX;

    // 続いてスポットライト。
    uint lightStart = tileIndex * numSpotLight;
    uint lightEnd = lightStart + numSpotLight;
    for (uint lightListIndex = lightStart; lightListIndex < lightEnd; lightListIndex++)
    {
        uint ligNo = spotLightListInTile[lightListIndex];
        if (ligNo == 0xffffffff)
        {
            // このタイルに含まれるポイントライトはもうない
            break;
        }
        
        float3 ligDir = normalize(worldPos - spotLight[ligNo].position);
        // 2. 光源からサーフェイスまでの距離を計算
        float distance = length(worldPos - spotLight[ligNo].position);
        float3 ligColor = spotLight[ligNo].color;
        float3 ptLig = CalcLighting(
            ligDir,
            ligColor,
            normal,
            toEye,
            albedoColor,
            metaric,
            smooth,
            specColor
        );
        // 3. 影響率を計算する。影響率は0.0～1.0の範囲で、
        //     指定した距離（pointsLights[i].range）を超えたら、影響率は0.0になる
        float affect = 1.0f - min(1.0f, distance / spotLight[ligNo].attn.x);

        // 入射光と射出方向の角度による減衰を計算する
        // dot()を利用して内積を求める
        float angle = dot(ligDir, spotLight[ligNo].direction);
        // dot()で求めた値をacos()に渡して角度を求める
        angle = abs(acos(angle));
        // step-12 角度による影響率を求める
        // 角度に比例して小さくなっていく影響率を計算する
        float angleAffect = max( 0.0f, 1.0f - 1.0f / spotLight[ligNo].attn.z * angle );
        affect *= angleAffect;

        affect = pow( affect, spotLight[ligNo].attn.y );
        lig += ptLig * affect * angleAffect;
    }
    return lig;
}
//ピクセルシェーダーのコア。
float4 PSMainCore(PSInput In, uniform int isSoftShadow) 
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
    
    float2 viewportPos = In.pos.xy;

    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(eyePos - worldPos);
    
    // ディレクションライトを計算
    float3 lig = CalcDirectionLight(
        normal, 
        toEye, 
        albedoColor, 
        metaric, 
        smooth, 
        specColor, 
        worldPos,
        isSoftShadow,
        shadowParam
    );
    
    lig += CalcPointLight(
        normal, 
        toEye, 
        albedoColor, 
        metaric, 
        smooth, 
        specColor, 
        worldPos,
        viewportPos
    );
    
    lig += CalcSpotLight(
        normal, 
        toEye, 
        albedoColor, 
        metaric, 
        smooth, 
        specColor, 
        worldPos,
        viewportPos
    );

    // todo ボリュームライトのテスト
    float volumeFrontZ = g_volumeLightMapFront.Sample(Sampler, In.uv).r;
    float volumeBackZ = g_volumeLightMapBack.Sample(Sampler, In.uv).r;
    float3 volumePosBack = CalcWorldPosFromUVZ( In.uv, volumeBackZ, mViewProjInv);
    float3 volumePosFront = CalcWorldPosFromUVZ( In.uv, volumeFrontZ, mViewProjInv);
    
    float t0 = dot( spotLight[0].direction, volumePosFront - spotLight[0].position);
    float t1 = dot( spotLight[0].direction, volumePosBack - spotLight[0].position);
    float t = t0 / (t0 + t1);
    float3 volumeCenterPos = lerp( volumePosFront, volumePosBack, t);
    float volume = length(volumePosBack - volumePosFront);
    if( volume > 0.001f){
        float rayStep = volume / 4.0f;
        
        // 0番目のスポットライトとボリュームライトの中心座標を使って、適当に光の量を計算する。
        // 積分は後でね・・・。
        // 距離による減衰
        float3 ligDir = (volumeCenterPos - spotLight[0].position);
        float distance = length(ligDir);
        ligDir = normalize(ligDir);
        float affect = pow( 1.0f - min(1.0f, distance / spotLight[0].attn.x), spotLight[0].attn.y);
        
        // 続いて角度による減衰を計算する。
        // 角度に比例して小さくなっていく影響率を計算する
        float angle = dot(ligDir, spotLight[0].direction);
        // dot()で求めた値をacos()に渡して角度を求める
        angle = abs(acos(angle));
        float angleAffect = max( 0.0f, 1.0f - 1.0f / spotLight[0].attn.z * angle );
        affect *= pow( angleAffect, spotLight[0].attn.w);

        // ボリュームライトの中央地点の光の量を計算する。
        lig += albedoColor * spotLight[0].color * affect * step( volumeFrontZ, albedoColor.w ) * volume * 0.01f;
        /*float3 centerLig = albedoColor * spotLight[0].color * affect * step( volumeFrontZ, albedoColor.w ) ;
        float3 prevLig = centerLig ;
        
        // 正の方向にレイマーチングを進める。
        {
            float3 rayDirection = (volumePosFront - volumeCenterPos ) ;
            // 正の方向のボリュームを求める。
            volume = max( 0.001f, length(rayDirection) );
            rayDirection /= volume;
            // レイステップの長さ。
            int numRayStep = 3;
            // 一つのステップの長さを計算する。
            float rayStepLen = volume / (numRayStep + 1); 
            for( int rayStep = 0; rayStep < numRayStep; rayStep++){
                float pos = volumeCenterPos + rayDirection * rayStepLen * rayStep;
                // 0番目のスポットライトを使って、光の量を計算する。
                // 距離による減衰
                float3 ligDir = (pos - spotLight[0].position);
                float distance = length(ligDir);
                ligDir = normalize(ligDir);
                float affect = pow( 1.0f - min(1.0f, distance / spotLight[0].attn.x), spotLight[0].attn.y);
                
                // 続いて角度による減衰を計算する。
                // 角度に比例して小さくなっていく影響率を計算する
                float angle = dot(ligDir, spotLight[0].direction);
                // dot()で求めた値をacos()に渡して角度を求める
                angle = abs(acos(angle));
                float angleAffect = max( 0.0f, 1.0f - 1.0f / spotLight[0].attn.z * angle );
                affect *= pow( angleAffect, spotLight[0].attn.w);

                // ボリュームライトのこの地点の光の量を計算する。
                float3 currentLig = spotLight[0].color * affect * step( volumeFrontZ, albedoColor.w ) ;
                // 一つ前のステップの光の量と、この地点の光の量を使って、
                // 光の量を積分する。
                lig += albedoColor * (prevLig + currentLig) * rayStepLen * 0.5f; // 0.001fは大気中のホコリの密度。
                prevLig = currentLig;

            }
        }
        prevLig = centerLig ;
        // 負の方向にレイマーチングを進める。
        {
            float3 rayDirection = (volumePosBack - volumeCenterPos ) ;
            // 正の方向のボリュームを求める。
            volume = max( 0.001f, length(rayDirection) );
            rayDirection /= volume;
            // レイステップの長さ。
            int numRayStep = 3;
            // 一つのステップの長さを計算する。
            float rayStepLen = volume / (numRayStep + 1); 
            for( int rayStep = 0; rayStep < numRayStep; rayStep++){
                float pos = volumeCenterPos + rayDirection * rayStepLen * rayStep;
                // 0番目のスポットライトを使って、光の量を計算する。
                // 距離による減衰
                float3 ligDir = (pos - spotLight[0].position);
                float distance = length(ligDir);
                ligDir = normalize(ligDir);
                float affect = pow( 1.0f - min(1.0f, distance / spotLight[0].attn.x), spotLight[0].attn.y);
                
                // 続いて角度による減衰を計算する。
                // 角度に比例して小さくなっていく影響率を計算する
                float angle = dot(ligDir, spotLight[0].direction);
                // dot()で求めた値をacos()に渡して角度を求める
                angle = abs(acos(angle));
                float angleAffect = max( 0.0f, 1.0f - 1.0f / spotLight[0].attn.z * angle );
                affect *= pow( angleAffect, spotLight[0].attn.w);

                // ボリュームライトのこの地点の光の量を計算する。
                float3 currentLig = spotLight[0].color * affect * step( volumeFrontZ, albedoColor.w ) ;
                // 一つ前のステップの光の量と、この地点の光の量を使って、
                // 光の量を積分する。
                lig += albedoColor * (prevLig + currentLig) * rayStepLen * 0.5f; // 0.001fは大気中のホコリの密度。
                prevLig = currentLig;

            }
        }*/
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