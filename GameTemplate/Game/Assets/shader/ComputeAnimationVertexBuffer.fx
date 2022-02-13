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
    float4x4 worldMatrix;   // ワールド行列。
    int numVertex ;         // 頂点数。    
}

StructuredBuffer<SVertex> g_iputVertexBuffer : register(t0);
StructuredBuffer<float4x4> g_boneMatrix         : register(t1);	    //ボーン行列。
RWStructuredBuffer<SVertex> g_outputVertexBuffer : register(u0);

/*!
 * @brief CSMain
 */
[numthreads(256, 1, 1)]
void CSMain(
    uint3 groupId          : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId    : SV_GroupThreadID)
{
    if( dispatchThreadId.x < numVertex){
        SVertex inVertex = g_iputVertexBuffer[dispatchThreadId.x];
        float4x4 worldMatrixLocal = 0;	
        if( inVertex.skinWeights[0] > 0.0f){
            // スキンあり。            
            float w = 0.0f;
            [unroll]
            for (int i = 0; i < 3; i++)
            {
                worldMatrixLocal += g_boneMatrix[inVertex.indices[i]] * inVertex.skinWeights[i];
                w += inVertex.skinWeights[i];
            }

            worldMatrixLocal += g_boneMatrix[inVertex.indices[3]] * (1.0f - w);
        }else{
            // スキンなし。
            worldMatrixLocal = worldMatrix;
        }
        g_outputVertexBuffer[dispatchThreadId.x] = inVertex;
        g_outputVertexBuffer[dispatchThreadId.x].pos = mul( worldMatrixLocal, float4(inVertex.pos, 1.0f) );
        g_outputVertexBuffer[dispatchThreadId.x].normal = mul( worldMatrixLocal, inVertex.normal);
        g_outputVertexBuffer[dispatchThreadId.x].tangent = mul( worldMatrixLocal, inVertex.tangent);
        g_outputVertexBuffer[dispatchThreadId.x].binormal = mul( worldMatrixLocal, inVertex.binormal);
    }
}
