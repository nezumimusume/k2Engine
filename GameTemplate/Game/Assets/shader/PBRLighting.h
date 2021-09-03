///////////////////////////////////////
// �L�����̃��C�e�B���O�֌W�̋��ʏ������܂Ƃ߂�B
// ���̃w�b�_�[��3dsMax���ł��g�p���Ă��܂��B
///////////////////////////////////////

///////////////////////////////////////
// �萔
///////////////////////////////////////
static const float PI = 3.1415926f;         // ��

// ������3dsMax�ł͕s�v�B
#ifndef _MAX_
static const int MAX_POINT_LIGHT = 1000;    // �|�C���g���C�g�̍ő吔

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

///////////////////////////////////////
// �\���́B
///////////////////////////////////////
// �f�B���N�V�������C�g�\���́B
struct DirectionalLight
{
    float3 direction;   // ���C�g�̕���
    int castShadow;     // �e���L���X�g����H
    float4 color;       // ���C�g�̐F
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

// ���C�g�p�̒萔�o�b�t�@�[
cbuffer LightCb : register(b1)
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    PointLight pointLight[MAX_POINT_LIGHT];
    float4x4 mViewProjInv;  // �r���[�v���W�F�N�V�����s��̋t�s��
    float3 eyePos;          // �J�����̎��_
    int numPointLight;      // �|�C���g���C�g�̐��B    
    float3 ambientLight;    // ����
    int isIBL;              // IBL���s���B
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
    float iblLuminance;     // IBL�̖��邳�B
};

///////////////////////////////////////
// �e�N�X�`��
///////////////////////////////////////


///////////////////////////////////////
// �T���v���X�e�[�g�B
///////////////////////////////////////
sampler Sampler : register(s0);

#endif // #ifdef _MAX_

///////////////////////////////////////
// �֐�
///////////////////////////////////////
#ifndef _MAX_
float CalcShadowRate(int ligNo, float3 worldPos, int isSoftShadow)
{
    float shadow = 0.0f;
    for(int cascadeIndex = 0; cascadeIndex < NUM_SHADOW_MAP; cascadeIndex++)
    {
        float4 posInLVP = mul( mlvp[ligNo][cascadeIndex], float4( worldPos, 1.0f ));
        float2 shadowMapUV = posInLVP.xy / posInLVP.w;
        float zInLVP = posInLVP.z / posInLVP.w;
        shadowMapUV *= float2(0.5f, -0.5f);
        shadowMapUV += 0.5f;
        // �V���h�E�}�b�vUV���͈͓�������
        if(shadowMapUV.x >= 0.0f && shadowMapUV.x <= 1.0f
            && shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f)
        {
            // �V���h�E�}�b�v����l���T���v�����O
            float3 shadowValue = g_shadowMap[ligNo][cascadeIndex].Sample(Sampler, shadowMapUV).xyz;

            // �܂����̃s�N�Z�����Օ�����Ă��邩���ׂ�
            if(zInLVP >= shadowValue.r + 0.001f)
            {
                if( isSoftShadow ){
                    // �\�t�g�V���h�E�B
                    // �Օ�����Ă���Ȃ�A�`�F�r�V�F�t�̕s�����𗘗p���Č���������m�������߂�
                    float depth_sq = shadowValue.x * shadowValue.x;
                    // ���̃O���[�v�̕��U������߂�
                    // ���U���傫���قǁAvariance�̐��l�͑傫���Ȃ�
                    float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);
                    // ���̃s�N�Z���̃��C�g���猩���[�x�l�ƃV���h�E�}�b�v�̕��ς̐[�x�l�̍������߂�
                    float md = zInLVP - shadowValue.x;
                    // �����͂��m�������߂�
                    float lit_factor = variance / (variance + md * md);
                    // �����͂��m������e�ɂȂ�m�����v�Z����B
                    shadow = 1.0f - pow( lit_factor, 5.0f ) ;
                }else{
                    // �n�[�h�V���h�E�B
                    shadow = 1.0f;
                }
            }
            break;
        }
    }
    return shadow;
}
#endif // #ifdef _MAX_

// �x�b�N�}�����z���v�Z����
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f-t2)/ t2);
    return D;
}

// �t���l�����v�Z�BSchlick�ߎ����g�p
float SpcFresnel(float f0, float u)
{
    // from Schlick
    return f0 + (1-f0) * pow(1-u, 5);
}

/// <summary>
/// �N�b�N�g�����X���f���̋��ʔ��˂��v�Z
/// </summary>
/// <param name="L">�����Ɍ������x�N�g��</param>
/// <param name="V">���_�Ɍ������x�N�g��</param>
/// <param name="N">�@���x�N�g��</param>
/// <param name="smooth">���炩��</param>
float CookTorranceSpecular(float3 L, float3 V, float3 N, float smooth)
{
    float microfacet = 1.0f - smooth;

    // �����x�𐂒����˂̎��̃t���l�����˗��Ƃ��Ĉ���
    // �����x�������قǃt���l�����˂͑傫���Ȃ�
    float f0 = smooth;

    // ���C�g�Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
    float3 H = normalize(L + V);

    // �e��x�N�g�����ǂꂭ�炢���Ă��邩����ς𗘗p���ċ��߂�
    float NdotH = max( saturate(dot(N, H)), 0.001f );
    float VdotH = max( saturate(dot(V, H)), 0.001f );
    float NdotL = max( saturate(dot(N, L)), 0.001f );
    float NdotV = max( saturate(dot(N, V)), 0.001f );

    // D�����x�b�N�}�����z��p���Čv�Z����
    float D = Beckmann(microfacet, NdotH);

    // F����Schlick�ߎ���p���Čv�Z����
    float F = SpcFresnel(f0, VdotH);

    // G�������߂�
    float G = min(1.0f, min(2*NdotH*NdotV/VdotH, 2*NdotH*NdotL/VdotH));

    // m�������߂�
    float m = PI * NdotV * NdotH;

    // �����܂ŋ��߂��A�l�𗘗p���āA�N�b�N�g�����X���f���̋��ʔ��˂����߂�
    return max(F * D * G / m, 0.0);
}

/// <summary>
/// �t���l�����˂��l�������g�U���˂��v�Z
/// </summary>
/// <remark>
/// ���̊֐��̓t���l�����˂��l�������g�U���˗����v�Z���܂�
/// �t���l�����˂́A�������̂̕\�ʂŔ��˂��錻�ۂ̂Ƃ��ŁA���ʔ��˂̋����ɂȂ�܂�
/// ����g�U���˂́A�������̂̓����ɓ����āA�����������N�����āA�g�U���Ĕ��˂��Ă������̂��Ƃł�
/// �܂�t���l�����˂��ア�Ƃ��ɂ́A�g�U���˂��傫���Ȃ�A�t���l�����˂������Ƃ��́A�g�U���˂��������Ȃ�܂�
///
/// </remark>
/// <param name="N">�@��</param>
/// <param name="L">�����Ɍ������x�N�g���B���̕����Ƌt�����̃x�N�g���B</param>
/// <param name="V">�����Ɍ������x�N�g���B</param>
/// <param name="roughness">�e���B0�`1�͈̔́B</param>
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V, float smooth)
{
    // step-1 �f�B�Y�j�[�x�[�X�̃t���l�����˂ɂ��g�U���˂�^�ʖڂɎ�������B
    // �����Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
    float3 H = normalize(L+V);
    
    //�e����0.5�ŌŒ�B
    float roughness = 1.0f - smooth;
    
    //�����
    float energyBias = lerp(0.0f, 0.5f, roughness);
    float energyFactor = lerp(1.0, 1.0/1.51, roughness);

    // �����Ɍ������x�N�g���ƃn�[�t�x�N�g�����ǂꂾ�����Ă��邩����ςŋ��߂�
    float dotLH = saturate(dot(L,H));
    // �����Ɍ������x�N�g���ƃn�[�t�x�N�g���A�������s�ɓ��˂����Ƃ��̊g�U���˗ʂ����߂Ă���B
    float Fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;
    
    // �@���ƌ����Ɍ������x�N�g�����𗘗p���Ċg�U���˗������߂Ă��܂�
    float dotNL = saturate(dot(N,L));
    float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

    
    // �@���Ǝ��_�Ɍ������x�N�g���𗘗p���Ċg�U���˗������߂Ă��܂�
    float dotNV = saturate(dot(N,V));
    float FV =  (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

    //�@���ƌ����ւ̕����Ɉˑ�����g�U���˗��ƁA�@���Ǝ��_�x�N�g���Ɉˑ�����g�U���˗���
    // ��Z���čŏI�I�Ȋg�U���˗������߂Ă���BPI�ŏ��Z���Ă���̂͐��K�����s������
    return (FL*FV * energyFactor);
}



// ���ˌ����v�Z����B
float3 CalcLighting(
    float3 ligDir, 
    float3 ligColor, 
    float3 normal,
    float3 toEye, 
    float4 albedoColor,  
    float metaric, 
    float smooth, 
    float3 specColor
    )
{
    // �e�������Ă��Ȃ��̂Ń��C�g�̌v�Z���s���B
    // �f�B�Y�j�[�x�[�X�̊g�U���˂���������
    // �t���l�����˂��l�������g�U���˂��v�Z
    float diffuseFromFresnel = CalcDiffuseFromFresnel(
        normal, -ligDir, toEye, smooth);

    // ���K��Lambert�g�U���˂����߂�
    float NdotL = saturate(dot(normal, -ligDir));
    float3 lambertDiffuse = ligColor * NdotL / PI;

    // �ŏI�I�Ȋg�U���ˌ����v�Z����
    float3 diffuse = albedoColor * diffuseFromFresnel * lambertDiffuse;

    // �N�b�N�g�����X���f���𗘗p�������ʔ��˗����v�Z����
    // �N�b�N�g�����X���f���̋��ʔ��˗����v�Z����
    float3 spec = CookTorranceSpecular(
        -ligDir, toEye, normal, smooth)
        * ligColor;

    // �����x��������΁A���ʔ��˂̓X�y�L�����J���[�A�Ⴏ��Δ�
    // �X�y�L�����J���[�̋��������ʔ��˗��Ƃ��Ĉ���

    spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metaric);

    // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
    return max( float3( 0.0f, 0.0f, 0.0f ), diffuse * (1.0f - smooth) + spec * smooth );   
}

