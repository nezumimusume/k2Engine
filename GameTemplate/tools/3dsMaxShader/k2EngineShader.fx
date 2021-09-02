// k2Engineのデフォルトシェーダー。

/////////////////////////////////////////////
// ユーザー定義の定数
static const float PI = 3.1415926f;         // π

/////////////////////////////////////////////
// max定数
// max定数は下記のURLを参照
// https://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_0C7A600E_7954_42B0_86EE_586A379A2CAD_htm
/////////////////////////////////////////////

// ライトの方向
float3 lightDir : Direction <
    string UIName = "Light Direction";
    string Object = "TargetLight";
> = { -0.577, -0.577, 0.577 };

float3 lightColor : LIGHTCOLOR;

// transformations
cbuffer everyFrame
{
    matrix World      : 		WORLD;
    matrix WorldIT   : 		WORLDINVERSETRANSPOSE;
    matrix WorldViewProj : 		WORLDVIEWPROJ;
    matrix WorldView 	: 		WORLDVIEW;
    matrix Projection 	: 		PROJECTION;
    matrix ViewI		:		VIEWINVERSE;
};

/////////////////////////////////////////////
// 構造体
/////////////////////////////////////////////

// 頂点シェーダーへの入力。
struct VS_INPUT
{
    float3 Pos  : POSITION;     // 座標
    float3 normal : NORMAL;     // 法線
    float3 tangent : TANGENT;   // 接ベクトル
    float3 biNormal : BINORMAL;     //従ベクトル。
    float2 uv	: TEXCOORD0;	// UV座標。
};

// 頂点シェーダーからの出力
struct VS_OUTPUT
{
    float4 Pos  : SV_Position;  // 座標
    float2 uv	: TEXCOORD0;	// UV座標
    float3 normal : NORMAL;     // 法線。
    float3 tangent  : TANGENT;  //接ベクトル。
    float3 biNormal : BINORMAL; //従ベクトル。
    float3 worldPos : TEXCOORD1;    // ワールド座標。
};



/////////////////////////////////////////////
// パイプラインステート
/////////////////////////////////////////////

RasterizerState DataCulling
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockwise = false;
};

DepthStencilState Depth
{
    DepthEnable = TRUE;
    DepthFunc = 4;//LESSEQUAL
    DepthWriteMask = ALL;
};

/////////////////////////////////////////////
// テクスチャ
/////////////////////////////////////////////

// アルベドマップ
Texture2D<float4> g_albedo : AlbedoMap <
    string UIName = "Albedo";
    string ResourceType = "2D";
> ;
// 法線マップ
Texture2D<float4> g_normal : NormalMap <
    string UIName = "Normal";
    string ResourceType = "2D";
> ;

// メタリック+スムース
Texture2D<float4> g_metallicAndSmoothMap : MetallicSmoothMap <
    string UIName = "Metallic+Smooth";
    string ResourceType = "2D";
> ;

bool g_useNormalMap
<
    string UIName = "Use Normal Map";
> = false;

float3 g_ambient <
    string UIName = "Ambient";
> = { 0.5, 0.5, 0.5 };
/////////////////////////////////////////////
// サンプラ
/////////////////////////////////////////////
SamplerState g_sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// 頂点シェーダー
VS_OUTPUT VS(VS_INPUT In)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    Out.worldPos = mul(float4(In.Pos, 1), World);
    Out.Pos = mul(float4(In.Pos, 1), WorldViewProj);    	// position (projected)
    Out.normal = mul(In.normal, World);                 // 法線を回す。 
    Out.tangent = mul(In.tangent, World);                // 節ベクトルも回す。
    Out.biNormal = mul(In.biNormal, World);     // 従ベクトルを計算する。
    Out.uv = In.uv;

    return Out;
}

// 法線マップから法線を取得。
float3 GetNormalFromNormalMap(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = g_normal.Sample(g_sampler, uv).xyz;
    // サンプリングしたカラーにガンマ補正がかかっているので、打ち消すために逆補正。
    binSpaceNormal = pow(binSpaceNormal, 1.0f / 2.2f);
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float tmp = binSpaceNormal.y;
    binSpaceNormal.x *= -1.0f;
    binSpaceNormal.y = binSpaceNormal.z;
    binSpaceNormal.z = tmp;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.z + normal * binSpaceNormal.y;

    return newNormal;
}

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
    D *= exp((-1.0f / m2) * (1.0f - t2) / t2);
    return D;
}

// フレネルを計算。Schlick近似を使用
float SpcFresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1 - f0) * pow(1 - u, 5);
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
    float microfacet = 1.0f - smooth;

    // 金属度を垂直入射の時のフレネル反射率として扱う
    // 金属度が高いほどフレネル反射は大きくなる
    float f0 = smooth;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める
    float NdotH = max(saturate(dot(N, H)), 0.001f);
    float VdotH = max(saturate(dot(V, H)), 0.001f);
    float NdotL = max(saturate(dot(N, L)), 0.001f);
    float NdotV = max(saturate(dot(N, V)), 0.001f);

    // D項をベックマン分布を用いて計算する
    float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する
    float F = SpcFresnel(f0, VdotH);

    // G項を求める
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

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
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V, float smooth)
{
    // step-1 ディズニーベースのフレネル反射による拡散反射を真面目に実装する。
    // 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);

    //粗さは0.5で固定。
    float roughness = 1.0f - smooth;

    //これは
    float energyBias = lerp(0.0f, 0.5f, roughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

    // 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める
    float dotLH = saturate(dot(L, H));
    // 光源に向かうベクトルとハーフベクトル、光が平行に入射したときの拡散反射量を求めている。
    float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

    // 法線と光源に向かうベクトルｗを利用して拡散反射率を求めています
    float dotNL = saturate(dot(N, L));
    float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));


    // 法線と視点に向かうベクトルを利用して拡散反射率を求めています
    float dotNV = saturate(dot(N, V));
    float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

    //法線と光源への方向に依存する拡散反射率と、法線と視点ベクトルに依存する拡散反射率を
    // 乗算して最終的な拡散反射率を求めている。PIで除算しているのは正規化を行うため
    return (FL * FV * energyFactor);
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
        normal, -ligDir, toEye, smooth);

    // 正規化Lambert拡散反射を求める
    float NdotL = saturate(dot(normal, -ligDir));
    float3 lambertDiffuse = ligColor * NdotL / PI;
    return lambertDiffuse * albedoColor;
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
    return max(float3(0.0f, 0.0f, 0.0f), diffuse * (1.0f - smooth) + spec * smooth);
}


// ピクセルシェーダー
float4 PS(VS_OUTPUT In) : SV_Target
{
    float4 albedoColor = g_albedo.Sample(g_sampler, In.uv);
    float3 normal;
    if (g_useNormalMap) {
        normal = GetNormalFromNormalMap(
            normalize(In.normal),
            normalize(In.tangent),
            normalize(In.biNormal),
            In.uv
        );
    }
    else {
        normal = In.normal;
    }
    float4 metaricAndSmooth = g_metallicAndSmoothMap.Sample(g_sampler, In.uv);

    float3 toEye = ViewI[3].xyz - In.worldPos;
    float4 finalColor;
    finalColor.xyz = CalcLighting(
        -lightDir,
        lightColor,
        normal,
        toEye,
        albedoColor,
        metaricAndSmooth.r,                 // todo
        metaricAndSmooth.a,                 // todo
        albedoColor
    );

   
    finalColor.xyz += albedoColor.xyz * g_ambient ;
    finalColor.w = 1.0f;

    return finalColor;
}

technique10 Standard
{
    pass P0
    {
        SetRasterizerState(DataCulling);
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}