/*!
 * @brief �u���[��
 */

#include "../util/ColorSpace.h"

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

/*!
 * @brief ���_�V�F�[�_�[
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

Texture2D<float4> mainRenderTargetTexture : register(t0);   // ���C�������_�����O�^�[�Q�b�g�̃e�N�X�`��
Texture2D<float4> luminanceAvgTexture : register(t1);       // �V�[���̕��ϋP�x���L������Ă���e�N�X�`���B
sampler Sampler : register(s0);

/////////////////////////////////////////////////////////
// �P�x���o�p
/////////////////////////////////////////////////////////
cbuffer SamplingLuminanceCb : register(b1)
{
    int isEnableTonemap;
    float middleGray;
    float threshold;
};

/*!
 * @brief �P�x���o�p�̃s�N�Z���V�F�[�_�[
 */
float4 PSSamplingLuminance(PSInput In) : SV_Target0
{
    // ���C�������_�����O�^�[�Q�b�g����J���[���T���v�����O
    float4 color = mainRenderTargetTexture.Sample(Sampler, In.uv);

    // �T���v�����O�����J���[�̖��邳���v�Z
    // ���C���V�[���̋P�x�����߂�B
    float luminance = luminanceAvgTexture.Sample(Sampler, In.uv);
    float3 hsv = Rgb2Hsv(color);
    if(isEnableTonemap){
        
        hsv.z = ( middleGray / ( max(luminance, 0.001f ))) * hsv.z;
    }
    // clip()�֐��͈����̒l���}�C�i�X�ɂȂ�ƁA�ȍ~�̏������X�L�b�v����
    // �Ȃ̂ŁA�}�C�i�X�ɂȂ�ƃs�N�Z���J���[�͏o�͂���Ȃ�
    
    clip(hsv.z - threshold );
    hsv.z -= threshold;
    
    if(isEnableTonemap){
        // �J���[�����̃J���[�ɖ߂��B
        hsv.z *= ( max(luminance, 0.001f )) / 0.18f;
    }
    color.xyz = Hsv2Rgb(hsv);
    return color;
}

// step-5 4���̃{�P�摜�ɃA�N�Z�X���邽�߂̕ϐ���ǉ�
Texture2D<float4> g_bokeTexture_0 : register(t0);
Texture2D<float4> g_bokeTexture_1 : register(t1);
Texture2D<float4> g_bokeTexture_2 : register(t2);
Texture2D<float4> g_bokeTexture_3 : register(t3);

float4 PSBloomFinal(PSInput In) : SV_Target0
{
    // step-6 �{�P�摜���T���v�����O���āA���ς��Ƃ��ďo�͂���
    float4 combineColor = g_bokeTexture_0.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_1.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_2.Sample(Sampler, In.uv);
    combineColor += g_bokeTexture_3.Sample(Sampler, In.uv);
    combineColor /= 4.0f;
    combineColor.a = 1.0f;
    return combineColor;
}
