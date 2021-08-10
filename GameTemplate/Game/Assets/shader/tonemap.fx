/*!
 * @brief	�g�[���}�b�v�B
 */

struct VSInput{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD0;
};
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

//�g�[���}�b�v�̋��ʒ萔�o�b�t�@�B
cbuffer cbTonemapCommon : register(b1){
	float deltaTime;
	float middleGray;
}

/*!
 * @brief	���_�V�F�[�_�[�B
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = In.pos;
	psIn.uv = In.uv;
	return psIn;
}

////////////////////////////////////////////////////////
// �P�x�̑ΐ����ς����߂�B
////////////////////////////////////////////////////////

static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);
Texture2D<float4> sceneTexture : register(t0);	//�V�[���e�N�X�`���B
sampler Sampler : register(s0);

static const int    MAX_SAMPLES            = 16;    // Maximum texture grabs

/*!
 * @brief	�萔�o�b�t�@�B
 */
cbuffer cbCalcLuminanceLog : register(b0){
	float4 g_avSampleOffsets[MAX_SAMPLES];
};
/*!
 *@brief	�P�x�̑ΐ����ς����߂�B
 */
float4 PSCalcLuminanceLogAvarage(PSInput In) : SV_Target0
{
	float3 vSample = 0.0f;
    float  fLogLumSum = 0.0f;

    for(int iSample = 0; iSample < 9; iSample++)
    {
        // Compute the sum of log(luminance) throughout the sample points
        vSample = sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy);
        float d = dot(vSample, LUMINANCE_VECTOR)+0.0001f;
        fLogLumSum += log(d);
    }
    
    // Divide the sum to complete the average
    fLogLumSum /= 9;

    return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);
}
////////////////////////////////////////////////////////
// �P�x�̕��ς����߂�B
////////////////////////////////////////////////////////
/*!
 *@brief	���ϋP�x�v�Z�s�N�Z���V�F�[�_�[�B
 */
float4 PSCalcLuminanceAvarage(PSInput In) : SV_Target0
{
	float fResampleSum = 0.0f; 
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy);
    }
    
    // Divide the sum to complete the average
    fResampleSum /= 16;

    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// �w���֐����g�p���ĕ��ϋP�x�����߂�
/////////////////////////////////////////////////////////
/*!
 *@brief	�w���֐����g�p���ĕ��ϋP�x�����߂�s�N�Z���V�F�[�_�[�B
 */
float4 PSCalcLuminanceExpAvarage( PSInput In ) : SV_Target0
{
	float fResampleSum = 0.0f;
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample]);
    }
    
    // Divide the sum to complete the average, and perform an exp() to complete
    // the average luminance calculation
    fResampleSum = exp(fResampleSum/16);
    
    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// ���Ï���
/////////////////////////////////////////////////////////

Texture2D<float4> lumAvgTexture : register(t1);		//���ϋP�x
Texture2D<float4> lastLumAvgTexture : register(t2);	//��t���[���O�̕��ϋP�x

/*!
 *@brief	���Ï����̂��߂̕��ϋP�x�̓K��������s�N�Z���V�F�[�_�[�B
 */
float4 PSCalcAdaptedLuminance( PSInput In ) : SV_Target0
{
	float fAdaptedLum = lastLumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    float fCurrentLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    
    // The user's adapted luminance level is simulated by closing the gap between
    // adapted luminance and current luminance by 2% every frame, based on a
    // 30 fps rate. This is not an accurate model of human adaptation, which can
    // take longer than half an hour.
    float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, 60 * deltaTime ) );
    return float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);
}
float4 PSCalcAdaptedLuminanceFirst(PSInput In) : SV_Target0
{
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
	return float4(fAvgLum, fAvgLum, fAvgLum, 1.0f);
}

/*!
 *@brief	���ϋP�x����g�[���}�b�v���s���s�N�Z���V�F�[�_�[�B
 */
float4 PSFinal( PSInput In) : SV_Target0
{
	float4 vSample = sceneTexture.Sample(Sampler, In.uv );
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2( 0.5f, 0.5f));
	
	vSample.rgb *= middleGray/max( 0.0001f, fAvgLum );
//	vSample.rgb /= (1.0f+vSample);
	return vSample;
}