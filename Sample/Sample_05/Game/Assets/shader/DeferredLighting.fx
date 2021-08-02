///////////////////////////////////////
// PBRベースのディファードライティング
///////////////////////////////////////

///////////////////////////////////////
// 定数
///////////////////////////////////////
static const int NUM_DIRECTIONAL_LIGHT = 4; // ディレクションライトの本数
static const int MAX_POINT_LIGHT = 1000;    // ポイントライトの最大数
static const float PI = 3.1415926f;         // π
static const int NUM_SHADOW_MAP = 3;        // シャドウマップの枚数。

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

///////////////////////////////////////
// 構造体。
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

// ライト用の定数バッファー
cbuffer LightCb : register(b1)
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    PointLight pointLight[MAX_POINT_LIGHT];
    float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
    float3 eyePos;          // カメラの視点
    int numPointLight;      // ポイントライトの数。    
    float3 ambientLight;    // 環境光
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
};

///////////////////////////////////////
// テクスチャ
///////////////////////////////////////
Texture2D<float4> albedoTexture : register(t0);     // アルベド
Texture2D<float4> normalTexture : register(t1);     // 法線
Texture2D<float4> worldPosTexture : register(t2);   // ワールド座標
Texture2D<float4> specularTexture : register(t3);   // スペキュラマップ。rgbにスペキュラカラー、aに金属度
Texture2D<float4> shadowParamTexture : register(t4);   // スペキュラマップ。rgbにスペキュラカラー、aに金属度
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t5);  //シャドウマップ。
// タイルごとのポイントライトのインデックスのリスト
StructuredBuffer<uint> pointLightListInTile : register(t20);

///////////////////////////////////////
// サンプラステート。
///////////////////////////////////////
sampler Sampler : register(s0);

///////////////////////////////////////
// 関数
///////////////////////////////////////
// ベックマン分布を計算する
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f-t2)/ t2);
    return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1-f0) * pow(1-u, 5);
}

/// <summary>
/// クックトランスモデルの鏡面反射を計算
/// </summary>
/// <param name="L">光源に向かうベクトル</param>
/// <param name="V">視点に向かうベクトル</param>
/// <param name="N">法線ベクトル</param>
/// <param name="smooth">滑らかさ</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float smooth)
{
    float microfacet = 0.76f;

    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
    float f0 = smooth;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

    // D項をベックマン分布を用いて計算する
    float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する
    float F = SpcFresnel(f0, VdotH);

    // G項を求める
    float G = min(1.0f, min(2*NdotH*NdotV/VdotH, 2*NdotH*NdotL/VdotH));

    // m項を求める
    float m = PI * NdotV * NdotH;

    // ここまで求めた、値を利用して、クックトランスモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}

/// <summary>
/// フレネル反射を考慮した拡散反射を計算
/// </summary>
/// <remark>
/// この関数はフレネル反射を考慮した拡散反射率を計算します
/// フレネル反射は、光が物体の表面で反射する現象のとこで、鏡面反射の強さになります
/// 一方拡散反射は、光が物体の内部に入って、内部錯乱を起こして、拡散して反射してきた光のことです
/// つまりフレネル反射が弱いときには、拡散反射が大きくなり、フレネル反射が強いときは、拡散反射が小さくなります
///
/// </remark>
/// <param name="N">法線</param>
/// <param name="L">光源に向かうベクトル。光の方向と逆向きのベクトル。</param>
/// <param name="V">視線に向かうベクトル。</param>
/// <param name="roughness">粗さ。0～1の範囲。</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-1 ディズニーベースのフレネル反射による拡散反射を真面目に実装する。
    // 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L+V);
    
    //粗さは0.5で固定。
    float roughness = 0.5f;
    
    //これは
    float energyBias = lerp(0.0f, 0.5f, roughness);
    float energyFactor = lerp(1.0, 1.0/1.51, roughness);

    // 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める
    float dotLH = saturate(dot(L,H));
    // 光源に向かうベクトルとハーフベクトル、光が平行に入射したときの拡散反射量を求めている。
    float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;
    
    // 法線と光源に向かうベクトルｗを利用して拡散反射率を求めています
    float dotNL = saturate(dot(N,L));
    float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

    
    // 法線と視点に向かうベクトルを利用して拡散反射率を求めています
    float dotNV = saturate(dot(N,V));
    float FV =  (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

    //法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
    return (FL*FV * energyFactor);
}

float CalcShadowRate(int ligNo, float3 worldPos)
{
    float shadow = 0.0f;
    for(int cascadeIndex = 0; cascadeIndex < NUM_SHADOW_MAP; cascadeIndex++)
    {
        float4 posInLVP = mul( mlvp[ligNo][cascadeIndex], float4( worldPos, 1.0f ));
        float2 shadowMapUV = posInLVP.xy / posInLVP.w;
        float zInLVP = posInLVP.z / posInLVP.w;
        shadowMapUV *= float2(0.5f, -0.5f);
        shadowMapUV += 0.5f;
        // シャドウマップUVが範囲内か判定
        if(shadowMapUV.x >= 0.0f && shadowMapUV.x <= 1.0f
            && shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f)
        {
            // シャドウマップから値をサンプリング
            float2 shadowValue = g_shadowMap[ligNo][cascadeIndex].Sample(Sampler, shadowMapUV).xy;

            // まずこのピクセルが遮蔽されているか調べる
            if(zInLVP >= shadowValue.r + 0.001f)
            {
                shadow = 1.0f;
            }
            break;
        }
    }
    return shadow;
}

//頂点シェーダー。
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

// 反射光を計算する。
float3 CalcLighting(
    float3 ligDir, 
    float3 ligColor, 
    float3 normal,
    float3 toEye, 
    float4 albedoColor,  
    float metaric, 
    float smooth, 
    float3 specColor
    )
{
    // 影が落ちていないのでライトの計算を行う。
    // ディズニーベースの拡散反射を実装する
    // フレネル反射を考慮した拡散反射を計算
    float diffuseFromFresnel = CalcDiffuseFromFresnel(
        normal, -ligDir, toEye);

    // 正規化Lambert拡散反射を求める
    float NdotL = saturate(dot(normal, -ligDir));
    float3 lambertDiffuse = ligColor * NdotL / PI;

    // 最終的な拡散反射光を計算する
    float3 diffuse = albedoColor * diffuseFromFresnel * lambertDiffuse;

    // クックトランスモデルを利用した鏡面反射率を計算する
    // クックトランスモデルの鏡面反射率を計算する
    float3 spec = CookTorranceSpecular(
        -ligDir, toEye, normal, smooth)
        * ligColor;

    // 金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白
    // スペキュラカラーの強さを鏡面反射率として扱う

    spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metaric);

    // 滑らかさを使って、拡散反射光と鏡面反射光を合成する
    return diffuse * (1.0f - smooth) + spec * smooth;   
}
//ピクセルシェーダー。
float4 PSMain(PSInput In) : SV_Target0
{
    //G-Bufferの内容を使ってライティング
    //アルベドカラーをサンプリング。
    float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);
    //法線をサンプリング。
    float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
    //ワールド座標をサンプリング。
    float3 worldPos = worldPosTexture.Sample(Sampler, In.uv).xyz;
    //スペキュラカラーをサンプリング。
    float3 specColor = albedoColor.xyz;
    //金属度をサンプリング。
    float metaric = specularTexture.SampleLevel(Sampler, In.uv, 0).r;
    //スムース
    float smooth = specularTexture.SampleLevel(Sampler, In.uv, 0).a;

    //影生成用のパラメータ。
    float4 shadowParam = shadowParamTexture.Sample(Sampler, In.uv);
    // 視線に向かって伸びるベクトルを計算する
    float3 toEye = normalize(eyePos - worldPos);

    float3 lig = 0;
    
    for(int ligNo = 0; ligNo < NUM_DIRECTIONAL_LIGHT; ligNo++)
    {
        // 影の落ち具合を計算する。
        float shadow = 0.0f;
        if( directionalLight[ligNo].castShadow == 1){
            //影を生成するなら。
            shadow = CalcShadowRate( ligNo, worldPos ) * shadowParam.r;
        }
        if( shadow < 0.9f){
            lig += CalcLighting(
                directionalLight[ligNo].direction,
                directionalLight[ligNo].color,
                normal,
                toEye,
                albedoColor,
                metaric,
                smooth,
                specColor
            );
        }
    }
    // 続いてポイントライト
    // スクリーンの左上を(0,0)、右下を(1,1)とする座標系に変換する
    // ビューポート座標系に変換する
    float2 viewportPos = In.pos.xy;

    // step-16 このピクセルが含まれているタイルの番号を計算する
    // スクリーンをタイルで分割したときのセルのX座標を求める
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;

    // タイルインデックスを計算する
    uint tileIndex = floor(viewportPos.x / TILE_WIDTH) + floor(viewportPos.y / TILE_HEIGHT) * numCellX;

    // step-17 含まれるタイルの影響リストの開始位置と終了位置を計算する
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
    // 環境光による底上げ
    lig += ambientLight * albedoColor;

    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return finalColor;
}

