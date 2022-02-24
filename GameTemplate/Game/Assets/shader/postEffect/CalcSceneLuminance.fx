/*!
 * @brief	�V�[���̕��ϋP�x�v�Z�p�̃V�F�[�_�[�B
 */

#include "../util/ColorSpace.h"

/*!
 *@brief    ���_�V�F�[�_�[�ւ̓���
 */
struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};
/*!
 *@brief    �s�N�Z���V�F�[�_�[�ւ̓���
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief    �X�v���C�g�`��̋��ʒ萔�o�b�t�@�B
 *@details  Sprite�N���X����b0���W�X�^�ɐݒ肳��Ă��܂��B
 */
cbuffer cb0 : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

/*!
 *@brief    ���ϋP�x�v�Z�p�̒萔�o�b�t�@�B
 */
cbuffer cb1 : register(b1){
	float deltaTime;    // �o�ߎ��ԁB
}

/*!
 * @brief	���_�V�F�[�_�[�B
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

////////////////////////////////////////////////////////
// �P�x�̑ΐ����ς����߂�B
////////////////////////////////////////////////////////

Texture2D<float4> sceneTexture : register(t0);	//�V�[���e�N�X�`���B
sampler Sampler : register(s0);

static const int    MAX_SAMPLES = 16;    // Maximum texture grabs

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

        vSample = max( sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy), 0.001f );
        float v = Rgb2V( vSample );
        fLogLumSum += log(v);
    }
    
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

Texture2D<float4> lumAvgTexture : register(t0);		                // ���݂̃t���[���̕��ϋP�x�B
Texture2D<float4> lumAvgInTonemapLastFrameTexture : register(t1);	// �P�t���[���O�̃g�[���}�b�v�Ŏg�p�������ϋP�x�B

/*!
 *@brief	���Ï����̂��߂̕��ϋP�x�̓K��������s�N�Z���V�F�[�_�[�B
 */
float4 PSCalcAdaptedLuminance( PSInput In ) : SV_Target0
{
	// 1�t���[���O�̃g�[���}�b�v�Ŏg�p�������ϋP�x���T���v�����O�B
	float lumAvgLastFrame = lumAvgInTonemapLastFrameTexture.Sample(Sampler, float2( 0.5f, 0.5f));
    // ���݂̃t���[���̕��ϋP�x���T���v�����O�B
    float lumAvgCurrentFrame = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    // ���`��Ԃł�����Ƃ����ϋP�x��ω������Ă����B
    float newLumAvg = lerp( lumAvgLastFrame, lumAvgCurrentFrame, 0.02f);
    return float4(newLumAvg, newLumAvg, newLumAvg, 1.0f);
}

float4 PSCalcAdaptedLuminanceFirst(PSInput In) : SV_Target0
{
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
	return float4(fAvgLum, fAvgLum, fAvgLum, 1.0f);
}