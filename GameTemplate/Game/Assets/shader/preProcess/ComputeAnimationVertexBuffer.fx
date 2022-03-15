/*!
 * @brief アニメーション済み頂点バッファを計算する処理。
 */


struct SVertex{
    float3 pos;			// 座標。
    float3 normal;		// 法線。
    float3 tangent;		// 接ベクトル。
    float3 binormal;	// 従ベクトル。
    float2 uv;			// UV座標。
    int4 indices;		// スキンインデックス。
    float4 skinWeights;	// スキンウェイト。
};

cbuffer cbParam : register(b0)
{
    float4x4 worldMatrix;   // ワールド行列。
    int numVertex ;         // 頂点数。    
    int numInstance;        // インスタンスの最大数。
}

StructuredBuffer<SVertex> g_iputVertexBuffer : register(t0);
StructuredBuffer<float4x4> g_boneMatrix         : register(t1);	    // ボーン行列。
StructuredBuffer<float4x4> g_worldMatrixArray   : register(t2);     // ワールド行列の配列。インスタンシングディスパッチの際に使われる。
RWStructuredBuffer<SVertex> g_outputVertexBuffer : register(u0);

#define NUM_THREAD_X        256
#define NUM_THREAD_Y        4
/*!
 * @brief CSMain
 */
[numthreads(NUM_THREAD_X, NUM_THREAD_Y, 1)]
void CSMain(
    uint3 groupId          : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId    : SV_GroupThreadID)
{
    int vertexNo = dispatchThreadId.x;
    int instanceNo = dispatchThreadId.y;
    if( vertexNo < numVertex
        && instanceNo < numInstance
    ){
        SVertex inVertex = g_iputVertexBuffer[vertexNo];
        float4x4 worldMatrixLocal = 0;	
        if( inVertex.skinWeights[0] > 0.0f){
            // スキンあり。            
            float4x4 skinMatrix = 0;
            float w = 0.0f;
            [unroll]
            for (int i = 0; i < 3; i++)
            {
                skinMatrix += g_boneMatrix[inVertex.indices[i]] * inVertex.skinWeights[i];
                w += inVertex.skinWeights[i];
            }

            skinMatrix += g_boneMatrix[inVertex.indices[3]] * (1.0f - w);
            if( numInstance == 1){
                // 一体の描画の場合はCPP側でスキン行列がワールド空間に変換されているので、
                // そのまま使う。
                worldMatrixLocal = skinMatrix;
            }else{
                // 複数のオブジェクトを描画する場合は通常の描画でスキン行列はモデル空間のままなので、
                // ここでワールド空間に変換する。
                worldMatrixLocal = mul( g_worldMatrixArray[instanceNo], skinMatrix);
            }
        }else{
            // スキンなし。
            if( numInstance == 1){
                worldMatrixLocal = worldMatrix;
            }else{
                worldMatrixLocal = g_worldMatrixArray[instanceNo];
            }
        }
        int outputOffset = instanceNo * numVertex;
        g_outputVertexBuffer[vertexNo + outputOffset] = inVertex;
        g_outputVertexBuffer[vertexNo + outputOffset].pos = mul( worldMatrixLocal, float4(inVertex.pos, 1.0f) );
        g_outputVertexBuffer[vertexNo + outputOffset].normal = normalize( mul( worldMatrixLocal, inVertex.normal) );
        g_outputVertexBuffer[vertexNo + outputOffset].tangent = normalize( mul( worldMatrixLocal, inVertex.tangent) );
        g_outputVertexBuffer[vertexNo + outputOffset].binormal = normalize( mul( worldMatrixLocal, inVertex.binormal) );
    }
}
