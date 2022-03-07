// �V���h�E�C���O�֌W�̏����B

#ifndef _SHADOWING_H_
#define _SHADOWING_H_

#include "Shadowing_const.h"
#include "PBRLighting_Const.h"

static const int INFINITY = 40.0f; 

///////////////////////////////////////
// �T���v���X�e�[�g�B
///////////////////////////////////////
#include "Sampler.h"

// �`�F�r�V�F�t�̕s�����𗘗p���āA�e�ɂȂ�\�����v�Z����B
float Chebyshev(float2 moments, float depth)
{
    if (depth <= moments.x) {
		return 0.0;
	}
    // �Օ�����Ă���Ȃ�A�`�F�r�V�F�t�̕s�����𗘗p���Č���������m�������߂�
    float depth_sq = moments.x * moments.x;
    // ���̃O���[�v�̕��U������߂�
    // ���U���傫���قǁAvariance�̐��l�͑傫���Ȃ�
    float variance = moments.y - depth_sq;
    // ���̃s�N�Z���̃��C�g���猩���[�x�l�ƃV���h�E�}�b�v�̕��ς̐[�x�l�̍������߂�
    float md = depth - moments.x;
    // �����͂��m�������߂�
    float lit_factor = variance / (variance + md * md);
    float lig_factor_min = 0.3f;
    // �����͂��m���̉����ȉ��͉e�ɂȂ�悤�ɂ���B
    lit_factor = saturate((lit_factor - lig_factor_min) / (1.0f - lig_factor_min));
    // �����͂��m������e�ɂȂ�m�������߂�B
    return 1.0f - lit_factor;
}
float CalcShadowRate(
    Texture2D<float4> shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP],
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP], 
    int ligNo, 
    float3 worldPos, 
    int isSoftShadow
)
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
            && shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f
            && zInLVP < 0.98f && zInLVP > 0.02f)
        {
            // �V���h�E�}�b�v����l���T���v�����O
            float4 shadowValue = shadowMap[ligNo][cascadeIndex].Sample(Sampler, shadowMapUV);
            zInLVP -= 0.001f;
            float pos = exp(INFINITY * zInLVP);
            if( isSoftShadow ){
                // �\�t�g�V���h�E�B
                shadow = Chebyshev(shadowValue.xy, pos);
            }else if(pos >= shadowValue.r ){
                // �n�[�h�V���h�E�B
                shadow = 1.0f;
            }
           
            break;
        }
    }
    return shadow;
}

#endif // _SHADOWING_H_