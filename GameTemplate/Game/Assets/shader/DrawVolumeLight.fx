///////////////////////////////////////
// �{�����[�����C�g�`��V�F�[�_�[�B
///////////////////////////////////////



// �V�[���̐[�x�e�N�X�`���B
Texture2D<float4> g_sceneDepthTexture : register(t10);

sampler Sampler : register(s0);


///////////////////////////////////////
// �\���́B
///////////////////////////////////////

// �s�N�Z���V�F�[�_�[�ւ̓���
struct SPSIn
{
	float4 pos : SV_POSITION;       //���W�B
	float4 posInProj : TEXCOORD0;
	float3 worldPos : TEXCOORD1;    // ���[���h���W
};


///////////////////////////////////////
// ���_�V�F�[�_�[�̋��ʏ������C���N���[�h����B
///////////////////////////////////////
#include "ModelVSCommon.h"

SPSIn VSMain(float4 pos : POSITION)
{
	SPSIn psIn;
	psIn.pos = mul(mWorld, pos); // ���f���̒��_�����[���h���W�n�ɕϊ�
	// ���_�V�F�[�_�[���烏�[���h���W���o��
	psIn.worldPos = pos;
	psIn.pos = mul(mView, psIn.pos); // ���[���h���W�n����J�������W�n�ɕϊ�
	psIn.pos = mul(mProj, psIn.pos); // �J�������W�n����X�N���[�����W�n�ɕϊ�
	psIn.posInProj = psIn.pos;

	return psIn;
}
/// <summary>
/// �������ސ[�x�l���v�Z����B
/// </summary>
float CalcDrawDepth(SPSIn psIn)
{
	float2 uv = psIn.posInProj.xy / psIn.posInProj.w;
	uv = uv * float2( 0.5f, -0.5f ) + float2( 0.5f, 0.5f );
	float sceneDepth = g_sceneDepthTexture.Sample(Sampler, uv).r;
	float z = min( sceneDepth, psIn.pos.z);
	return z;
}
/// <summary>
/// �O��/�w�ʕ`��p�̃s�N�Z���V�F�[�_�[�G���g���[�֐��B
/// </summary>
float4 PSMainFrontBack(SPSIn psIn) : SV_Target0
{
	// �{�����[�����C�g�}�b�v�ɏ������ސ[�x�l���v�Z����B
	float z = CalcDrawDepth(psIn);
	return float4( z, z, z, z);
}

/*!
 * @brief	UV���W�Ɛ[�x�l���烏�[���h���W���v�Z����B
 *@param[in]	uv				uv���W
 *@param[in]	zInScreen		�X�N���[�����W�n�̐[�x�l
 *@param[in]	mViewProjInv	�r���[�v���W�F�N�V�����s��̋t�s��B
 */
float3 CalcWorldPosFromUVZ( float2 uv, float zInScreen, float4x4 mViewProjInv )
{
	float3 screenPos;
	screenPos.xy = (uv * float2(2.0f, -2.0f)) + float2( -1.0f, 1.0f);
	screenPos.z = zInScreen;
	
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
///////////////////////////////////////
// �萔�o�b�t�@�B
///////////////////////////////////////


// �X�|�b�g���C�g
struct SpotLight
{
    float3 position;        // ���W
    int isUse;              // �g�p���t���O�B
    float3 positionInView;  // �J������Ԃł̍��W�B
    int no ;                // ���C�g�̔ԍ��B
    float3 direction;       // �ˏo�����B
    float range;            // �e���͈́B
    float3 color;           // ���C�g�̃J���[�B
    float3 color2;          // ��ڂ̃J���[�B
    float3 color3;          // �O�ڂ̃J���[�B
    float3 directionInView; // �J������Ԃł̎ˏo�����B
    float3 rangePow;        // �����ɂ����̉e�����ɗݏ悷��p�����[�^�[�B1.0�Ő��`�̕ω�������B
                            // x����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[�B
    float3 angle;           // �ˏo�p�x(�P�ʁF���W�A���Bx����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[)�B
    float3 anglePow;        // �X�|�b�g���C�g�Ƃ̊p�x�ɂ����̉e�����ɗݏ悷��p�����[�^�B1.0�Ő��`�ɕω�����B
                            // x����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[�B
};

// �|�C���g���C�g
struct PointLight
{
    float3 position;        // ���W
    float3 positionInView;  // �J������Ԃł̍��W
    float3 color;           // �J���[
    float3 attn;            // �����p�����[�^�B
};

///////////////////////////////////////
// �萔�o�b�t�@�B
///////////////////////////////////////

cbuffer cb : register(b0){
	float4x4 mvp;		// ���f���r���[�v���W�F�N�V�����s��
	float4 mulColor;	// ��Z�J���[
};

// �X�|�b�g���C�g�p�̒萔�o�b�t�@�[
cbuffer finalSpotLightCb : register(b1){
    SpotLight spotLight;    // �X�|�b�g���C�g�B
    float4x4 mViewProjInv;  // �r���[�v���W�F�N�V�����s��̋t�s��
    float randomSeed;   // �����_���V�[�h�B
    
};

// �|�C���g���C�g�p�̒萔�o�b�t�@�B
cbuffer finalPointLightCb : register(b1){
    PointLight pointLight;    // �X�|�b�g���C�g�B
    float4x4 mViewProjInv2;   // �r���[�v���W�F�N�V�����s��̋t�s��
    float randomSeed2;        // �����_���V�[�h�B
};

struct VSFinalInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct PSFinalInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> albedoTexture : register(t0);	// �A���x�h�J���[
Texture2D<float4> g_volumeLightMapFront : register(t1);
Texture2D<float4> g_volumeLightMapBack : register(t2);




PSFinalInput VSFinal(VSFinalInput In) 
{
	PSFinalInput psIn;
	psIn.pos = mul( mvp, In.pos );
	psIn.uv = In.uv;
	return psIn;
}
float GetRandomNumber(float2 texCoord, float Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}
float4 PSFinal_SpotLight( PSFinalInput In ) : SV_Target0
{
	float3 lig = 0;
	float2 uv = In.uv;
	
	float volumeFrontZ = g_volumeLightMapFront.Sample(Sampler, uv).r;
    float volumeBackZ = g_volumeLightMapBack.Sample(Sampler, uv).r;
  
    float3 volumePosBack = CalcWorldPosFromUVZ( uv, volumeBackZ, mViewProjInv);
    float3 volumePosFront = CalcWorldPosFromUVZ( uv, volumeFrontZ, mViewProjInv);
    

    float t0 = dot( spotLight.direction, volumePosFront - spotLight.position);
    float t1 = dot( spotLight.direction, volumePosBack - spotLight.position);
    float t = t0 / (t0 + t1);
    float3 volumeCenterPos = lerp( volumePosFront, volumePosBack, t);
    float volume = length(volumePosBack - volumePosFront);

    // �{�����[�����Ȃ��ӏ��̓s�N�Z���L���B
    clip( volume - 0.001f);
    
    float4 albedoColor = albedoTexture.Sample(Sampler, uv);
    
    // �����ɂ����̉e�������v�Z�B
    float3 ligDir = (volumeCenterPos - spotLight.position);
    float distance = length(ligDir);
    ligDir = normalize(ligDir);
    float3 affectBase = 1.0f - min(1.0f, distance / spotLight.range);
    float3 affect = pow( affectBase, spotLight.rangePow);     

    // �����Ċp�x�ɂ�錸�����v�Z����B
    // �p�x�ɔ�Ⴕ�ď������Ȃ��Ă����e�������v�Z����
    float angleLigToPixel = saturate(dot(ligDir, spotLight.direction) );
    
    // dot()�ŋ��߂��l��acos()�ɓn���Ċp�x�����߂�
    angleLigToPixel = abs(acos(angleLigToPixel)) ;
    
    // ���̊p�x�ɂ�錸�����v�Z�B
    float3 angleAffectBase = max( 0.0f, 1.0f - 1.0f / spotLight.angle * angleLigToPixel );
    angleAffectBase = min( 1.0f, angleAffectBase * 1.8f);
    float3 angleAffect = pow( angleAffectBase, spotLight.anglePow );    
    affect *= angleAffect;

    // �O�̌��������B    
    // ���̃x�[�X���v�Z�B
    float3 ligBase = albedoColor * step( volumeFrontZ, albedoColor.w ) * max( 0.0f, log(volume) ) * 0.1f;
    // ���̃x�[�X�ɉe��������Z����B
    lig = ligBase * affect.x * spotLight.color; 
    lig += ligBase * affect.y * spotLight.color2;
    lig += ligBase * affect.z * spotLight.color3;
    
    // ��C���̃`���̕\���Ƃ��ăm�C�Y��������B
    lig *= lerp( 0.9f, 1.1f, GetRandomNumber(uv, randomSeed));

	return float4( lig, 1.0f);
}
float4 PSFinal_PointLight( PSFinalInput In ) : SV_Target0
{
	float3 lig = 0;
	float2 uv = In.uv;
	
	float volumeFrontZ = g_volumeLightMapFront.Sample(Sampler, uv).r;
    float volumeBackZ = g_volumeLightMapBack.Sample(Sampler, uv).r;
  
    float3 volumePosBack = CalcWorldPosFromUVZ( uv, volumeBackZ, mViewProjInv2);
    float3 volumePosFront = CalcWorldPosFromUVZ( uv, volumeFrontZ, mViewProjInv2);
    

    
    float3 volumeCenterPos = (volumePosFront + volumePosBack ) * 0.5f;
    float volume = length(volumePosBack - volumePosFront);

    // �{�����[�����Ȃ��ӏ��̓s�N�Z���L���B
    clip( volume - 0.001f);

	 float4 albedoColor = albedoTexture.Sample(Sampler, uv);
    
    // �����ɂ�錸���B
    float3 ligDir = (volumeCenterPos - pointLight.position);
    float distance = length(ligDir);
    ligDir = normalize(ligDir);
    float affect = pow( 1.0f - min(1.0f, distance / pointLight.attn.x), pointLight.attn.y);

	lig = albedoColor * pointLight.color * affect * step( volumeFrontZ, albedoColor.w ) * max( 0.0f, log(volume) ) * 0.05f;
    lig *= lerp( 0.9f, 1.1f, GetRandomNumber(uv, randomSeed2));
	return float4( lig, 1.0f);
}