//モデルの頂点シェーダー関係の共通ヘッダー

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

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////

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

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
StructuredBuffer<float4x4> g_boneMatrix         : register(t3);	    //ボーン行列。
StructuredBuffer<float4x4> g_worldMatrixArray   : register(t10);	//ワールド行列の配列。インスタンシング描画の際に有効。
////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

/// <summary>
//スキン行列を計算する。
/// </summary>
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
