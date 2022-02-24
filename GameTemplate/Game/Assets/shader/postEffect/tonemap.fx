/*!
 * @brief	�g�[���}�b�v�B
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
 *@brief    �s�N�Z���V�F�[�_�[�ւ̓��́B
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief    �X�v���C�g�`��̋��ʒ萔�o�b�t�@�B
 *@details  Sprite�N���X����b0���W�X�^�ɐݒ肳��Ă��܂��B
 */
cbuffer SpriteCommonCb : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};


/*!
 *@brief    �g�[���}�b�v��p�̒萔�o�b�t�@�B
 */
cbuffer cb2 : register(b1){
	float middleGray;   
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


Texture2D<float4> sceneTexture : register(t0);	            // �V�[���e�N�X�`���B
Texture2D<float4> lumAvgInTonemapTexture : register(t1);    // �P�x�e�N�X�`���B
sampler Sampler : register(s0);

/*!
 *@brief    ACES���g�����g�[���}�b�p�[
 */
float ACESFilm(float x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

/*!
 *@brief	���ϋP�x����g�[���}�b�v���s���s�N�Z���V�F�[�_�[�B
 */
float4 PSFinal( PSInput In) : SV_Target0
{
	// �V�[���̃J���[���T���v�����O����B
	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv );
    // �V�[���̃J���[��RGB�n����HSV�n�ɕϊ�����B
    float3 hsv = Rgb2Hsv(sceneColor);

	float avgLum = lumAvgInTonemapTexture.Sample(Sampler, float2( 0.5f, 0.5f)).r;
    
    // ���ϋP�x��middleGray�̒l�ɂ��邽�߂̃X�P�[���l�����߂�B
    float k = ( middleGray / ( max(avgLum, 0.001f )));
    // �X�P�[���l���g���āA�P�x���X�P�[���_�E���B
    hsv.z *= k;
    
    // ACES�g�[���}�b�p�[���g���ċP�x�̕ω������`�ɂ���B
    hsv.z = ACESFilm(hsv.z);

    // HSV�n����RGB�n�ɖ߂��B
    sceneColor.xyz = Hsv2Rgb(hsv);
    
	return sceneColor;
}