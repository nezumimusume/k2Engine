// k2Engineのデフォルトシェーダー。

#include "PBRLighting.h"

/////////////////////////////////////////////
// max定数
// max定数は下記のURLを参照
// https://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_0C7A600E_7954_42B0_86EE_586A379A2CAD_htm
/////////////////////////////////////////////

// ライトの方向
float3 lightDir : Direction;

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
> = true;

bool g_useMetallicSmoothMap
<
    string UIName = "Use MetallicSmooth Map";
> = true;

float3 g_ambient  = { 0.5, 0.5, 0.5 };
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

    float3 toEye = normalize(ViewI[3].xyz - In.worldPos);
    float3 lightDirN = normalize(lightDir);
    float4 finalColor;
    finalColor.xyz = CalcLighting(
        -lightDirN,
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