/*!
 * @brief アニメーション済み頂点バッファを計算する処理。
 */


struct SVertex{
    float3 pos;			//座標。
    float3 normal;		//法線。
    float3 tangent;		//接ベクトル。
    float3 binormal;	//従ベクトル。
    float2 uv;			//UV座標。
    int4 indices;		//スキンインデックス。
    float4 skinWeights;	//スキンウェイト。
};

cbuffer cbParam : register(b0)
{
    int numVertex ; // 頂点数。    
}

StructuredBuffer<SVertex> g_iputVertexBuffer : register(t0);
StructuredBuffer<float4x4> g_boneMatrix         : register(t1);	    //ボーン行列。
RWStructuredBuffer<SVertex> g_outputVertexBuffer : register(u0);

/*!
 * @brief CSMain
 */
[numthreads(64, 1, 1)]
void CSMain(
    uint3 groupId          : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId    : SV_GroupThreadID)
{
    if( dispatchThreadId.x < numVertex){
        SVertex inVertex = g_iputVertexBuffer[dispatchThreadId.x];
        float4x4 skinning = 0;	
        float w = 0.0f;
        [unroll]
        for (int i = 0; i < 3; i++)
        {
            skinning += g_boneMatrix[inVertex.indices[i]] * inVertex.skinWeights[i];
            w += inVertex.skinWeights[i];
        }

        skinning += g_boneMatrix[inVertex.indices[3]] * (1.0f - w);
        g_outputVertexBuffer[dispatchThreadId.x].pos = mul( skinning, float4(inVertex.pos, 1.0f) );
    }
}
