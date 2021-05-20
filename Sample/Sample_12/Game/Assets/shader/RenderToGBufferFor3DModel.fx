///////////////////////////////////////
// 3Dモデル用のGBufferへの描画シェーダー。
///////////////////////////////////////


///////////////////////////////////////
// 構造体。
///////////////////////////////////////
// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos : POSITION;          //頂点座標。
    float3 normal : NORMAL;         //法線。
    float2 uv : TEXCOORD0;          //UV座標。
    float3 tangent  : TANGENT;      //接ベクトル。
    float3 biNormal : BINORMAL;     //従ベクトル。
    int4  Indices  	: BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;       //座標。
    float3 normal : NORMAL;         //法線。
    float3 tangent  : TANGENT;      //接ベクトル。
    float3 biNormal : BINORMAL;     //従ベクトル。
    float2 uv : TEXCOORD0;          //UV座標。
    float3 worldPos : TEXCOORD1;    // ワールド座標
};

// ピクセルシェーダーからの出力
struct SPSOut
{
    float4 albedo : SV_Target0;         // アルベド
    float4 normal : SV_Target1;         // 法線
    float4 worldPos : SV_Target2;       // ワールド座標
    float4 metaricSmooth : SV_Target3;  // メタリックとスムース。rにメタリック、aにスムース。
    float4 shadowParam : SV_Target4;    //影生成用パラメータ。
};

///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

///////////////////////////////////////
// シェーダーリソース
///////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);      //アルベドマップ
Texture2D<float4> g_normal : register(t1);      //法線マップ
Texture2D<float4> g_spacular : register(t2);    //スペキュラマップ

StructuredBuffer<float4x4> g_boneMatrix : register(t3);	//ボーン行列。

///////////////////////////////////////
// サンプラーステート
///////////////////////////////////////
sampler g_sampler : register(s0);

///////////////////////////////////////
// 関数
///////////////////////////////////////

// 法線マップから法線を取得。
float3 GetNormalFromNormalMap(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
    float3 binSpaceNormal = g_normal.SampleLevel (g_sampler, uv, 0.0f).xyz;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + biNormal * binSpaceNormal.y + normal * binSpaceNormal.z;
    
    return newNormal;
}

float4x4 CalcSkinMatrix(SVSIn skinVert)
{
	float4x4 skinning = 0;	
	float w = 0.0f;
	[unroll]
    for (int i = 0; i < 3; i++)
    {
        skinning += g_boneMatrix[skinVert.Indices[i]] * skinVert.Weights[i];
        w += skinVert.Weights[i];
    }
    
    skinning += g_boneMatrix[skinVert.Indices[3]] * (1.0f - w);
	
    return skinning;
}

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
    SPSIn psIn;
    float4x4 m;
    if( hasSkin ){
        m = CalcSkinMatrix(vsIn);
    }else{
        m = mWorld;
    }
    
    psIn.pos = mul(m, vsIn.pos); // モデルの頂点をワールド座標系に変換
    // 頂点シェーダーからワールド座標を出力
    psIn.worldPos = psIn.pos;

    psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
    psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
    psIn.normal = normalize(mul(mWorld, vsIn.normal));
    psIn.tangent = normalize(mul(mWorld, vsIn.tangent));
    psIn.biNormal = normalize(mul(mWorld, vsIn.biNormal));
    psIn.uv = vsIn.uv;

    return psIn;
}
SPSIn VSMain( SVSIn vsIn )
{
    return VSMainCore(vsIn, false);
}
SPSIn VSMainSkin( SVSIn vsIn )
{
    return VSMainCore(vsIn, true);
}
SPSOut PSMainCore( SPSIn psIn, int isShadowReciever)
{
    // G-Bufferに出力
    SPSOut psOut;
    // アルベドカラーを出力
    psOut.albedo = g_albedo.Sample(g_sampler, psIn.uv);
    // 法線を出力
    psOut.normal.xyz = GetNormalFromNormalMap( 
        psIn.normal, psIn.tangent, psIn.biNormal, psIn.uv ) ;
    psOut.normal.w = 1.0f;
    // メタリックスムースを出力。
    psOut.metaricSmooth = g_spacular.Sample(g_sampler, psIn.uv);
    // ワールド座標を出力。
    psOut.worldPos = float4( psIn.worldPos, 1.0f);
    // 影パラメータ。
    psOut.shadowParam = 255.0f * isShadowReciever;
    return psOut;
}
// モデル用のピクセルシェーダーのエントリーポイント
SPSOut PSMain(SPSIn psIn)
{
    return PSMainCore(psIn, 0);
}
SPSOut PSMainShadowReciever(SPSIn psIn)
{
    return PSMainCore(psIn, 1);
}
