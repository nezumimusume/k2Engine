///////////////////////////////////////
// PBR�x�[�X�̃f�B�t�@�[�h���C�e�B���O
///////////////////////////////////////

///////////////////////////////////////
// �萔
///////////////////////////////////////
static const int NUM_DIRECTIONAL_LIGHT = 4; // �f�B���N�V�������C�g�̖{��
static const float PI = 3.1415926f;         // ��
static const int NUM_SHADOW_MAP = 3;        // �V���h�E�}�b�v�̖����B
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
//���_�V�F�[�_�[�ւ̓��͍\���́B
struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};
//�s�N�Z���V�F�[�_�[�ւ̓��͍\���́B
struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

///////////////////////////////////////
// �萔�o�b�t�@�B
///////////////////////////////////////
//���ʒ萔�o�b�t�@
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
};

// ���C�g�p�̒萔�o�b�t�@�[
cbuffer LightCb : register(b1)
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    float3 eyePos;          // �J�����̎��_
    float specPow;          // �X�y�L�����̍i��
    float3 ambientLight;    // ����
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
};

///////////////////////////////////////
// �e�N�X�`��
///////////////////////////////////////
Texture2D<float4> albedoTexture : register(t0);     // �A���x�h
Texture2D<float4> normalTexture : register(t1);     // �@��
Texture2D<float4> worldPosTexture : register(t2);   // ���[���h���W
Texture2D<float4> specularTexture : register(t3);   // �X�y�L�����}�b�v�Brgb�ɃX�y�L�����J���[�Aa�ɋ����x
Texture2D<float4> shadowParamTexture : register(t4);   // �X�y�L�����}�b�v�Brgb�ɃX�y�L�����J���[�Aa�ɋ����x
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t5);  //�V���h�E�}�b�v�B
///////////////////////////////////////
// �T���v���X�e�[�g�B
///////////////////////////////////////
sampler Sampler : register(s0);

///////////////////////////////////////
// �֐�
///////////////////////////////////////
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
    float microfacet = 0.76f;

    // �����x�𐂒����˂̎��̃t���l�����˗��Ƃ��Ĉ���
    // �����x�������قǃt���l�����˂͑傫���Ȃ�
    float f0 = smooth;

    // ���C�g�Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
    float3 H = normalize(L + V);

    // �e��x�N�g�����ǂꂭ�炢���Ă��邩����ς𗘗p���ċ��߂�
    float NdotH = saturate(dot(N, H));
    float VdotH = saturate(dot(V, H));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));

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
float CalcDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // step-1 �f�B�Y�j�[�x�[�X�̃t���l�����˂ɂ��g�U���˂�^�ʖڂɎ�������B
    // �����Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂�
    float3 H = normalize(L+V);
    
    //�e����0.5�ŌŒ�B
    float roughness = 0.5f;
    
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

float CalcShadowRate(int ligNo, float3 worldPos)
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
            float2 shadowValue = g_shadowMap[ligNo][cascadeIndex].Sample(Sampler, shadowMapUV).xy;

            // �܂����̃s�N�Z�����Օ�����Ă��邩���ׂ�
            if(zInLVP >= shadowValue.r + 0.001f)
            {
                shadow = 1.0f;
            }
            break;
        }
    }
    return shadow;
}

//���_�V�F�[�_�[�B
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}



//�s�N�Z���V�F�[�_�[�B
float4 PSMain(PSInput In) : SV_Target0
{
    //G-Buffer�̓��e���g���ă��C�e�B���O
    //�A���x�h�J���[���T���v�����O�B
    float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);
    //�@�����T���v�����O�B
    float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
    //���[���h���W���T���v�����O�B
    float3 worldPos = worldPosTexture.Sample(Sampler, In.uv).xyz;
    //�X�y�L�����J���[���T���v�����O�B
    float3 specColor = albedoColor.xyz;
    //�����x���T���v�����O�B
    float metaric = specularTexture.SampleLevel(Sampler, In.uv, 0).r;
    //�X���[�X
    float smooth = specularTexture.SampleLevel(Sampler, In.uv, 0).a;

    //�e�����p�̃p�����[�^�B
    float4 shadowParam = shadowParamTexture.Sample(Sampler, In.uv);
    // �����Ɍ������ĐL�т�x�N�g�����v�Z����
    float3 toEye = normalize(eyePos - worldPos);

    float3 lig = 0;
    
    for(int ligNo = 0; ligNo < NUM_DIRECTIONAL_LIGHT; ligNo++)
    {
        // �e�̗�������v�Z����B
        float shadow = 0.0f;
        if( directionalLight[ligNo].castShadow == 1){
            //�e�𐶐�����Ȃ�B
            shadow = CalcShadowRate( ligNo, worldPos ) * shadowParam.r;
        }
        if( shadow < 0.9f){
            // �e�������Ă��Ȃ��̂Ń��C�g�̌v�Z���s���B
            // �f�B�Y�j�[�x�[�X�̊g�U���˂���������
            // �t���l�����˂��l�������g�U���˂��v�Z
            float diffuseFromFresnel = CalcDiffuseFromFresnel(
                normal, -directionalLight[ligNo].direction, toEye);

            // ���K��Lambert�g�U���˂����߂�
            float NdotL = saturate(dot(normal, -directionalLight[ligNo].direction));
            float3 lambertDiffuse = directionalLight[ligNo].color * NdotL / PI;

            // �ŏI�I�Ȋg�U���ˌ����v�Z����
            float3 diffuse = albedoColor * diffuseFromFresnel * lambertDiffuse;

            // �N�b�N�g�����X���f���𗘗p�������ʔ��˗����v�Z����
            // �N�b�N�g�����X���f���̋��ʔ��˗����v�Z����
            float3 spec = CookTorranceSpecular(
                -directionalLight[ligNo].direction, toEye, normal, smooth)
                * directionalLight[ligNo].color;

            // �����x��������΁A���ʔ��˂̓X�y�L�����J���[�A�Ⴏ��Δ�
            // �X�y�L�����J���[�̋��������ʔ��˗��Ƃ��Ĉ���
        
            spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metaric);

            // ���炩�����g���āA�g�U���ˌ��Ƌ��ʔ��ˌ�����������
            lig += diffuse * (1.0f - smooth) + spec * smooth;   
        }
    }
    // �����ɂ���グ
    lig += ambientLight * albedoColor;

    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return finalColor;
}

