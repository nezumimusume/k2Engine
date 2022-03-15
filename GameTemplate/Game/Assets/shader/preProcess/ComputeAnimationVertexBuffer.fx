/*!
 * @brief �A�j���[�V�����ςݒ��_�o�b�t�@���v�Z���鏈���B
 */


struct SVertex{
    float3 pos;			// ���W�B
    float3 normal;		// �@���B
    float3 tangent;		// �ڃx�N�g���B
    float3 binormal;	// �]�x�N�g���B
    float2 uv;			// UV���W�B
    int4 indices;		// �X�L���C���f�b�N�X�B
    float4 skinWeights;	// �X�L���E�F�C�g�B
};

cbuffer cbParam : register(b0)
{
    float4x4 worldMatrix;   // ���[���h�s��B
    int numVertex ;         // ���_���B    
    int numInstance;        // �C���X�^���X�̍ő吔�B
}

StructuredBuffer<SVertex> g_iputVertexBuffer : register(t0);
StructuredBuffer<float4x4> g_boneMatrix         : register(t1);	    // �{�[���s��B
StructuredBuffer<float4x4> g_worldMatrixArray   : register(t2);     // ���[���h�s��̔z��B�C���X�^���V���O�f�B�X�p�b�`�̍ۂɎg����B
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
            // �X�L������B            
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
                // ��̂̕`��̏ꍇ��CPP���ŃX�L���s�񂪃��[���h��Ԃɕϊ�����Ă���̂ŁA
                // ���̂܂܎g���B
                worldMatrixLocal = skinMatrix;
            }else{
                // �����̃I�u�W�F�N�g��`�悷��ꍇ�͒ʏ�̕`��ŃX�L���s��̓��f����Ԃ̂܂܂Ȃ̂ŁA
                // �����Ń��[���h��Ԃɕϊ�����B
                worldMatrixLocal = mul( g_worldMatrixArray[instanceNo], skinMatrix);
            }
        }else{
            // �X�L���Ȃ��B
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
