/////////////////////////////////////////////////////
// PBR���C�e�B���O�֌W�̍\���̂��܂Ƃ߂��w�b�_�[�t�@�C��
/////////////////////////////////////////////////////

#ifndef _PBRLIGHTING_STRUCT_H_
#define _PBRLIGHTING_STRUCT_H_

#include "Shadowing_const.h"

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

// �萔�o�b�t�@�Ŏg�p����f�[�^���܂Ƃ߂��\���́B
struct LightCB{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    PointLight pointLight[MAX_POINT_LIGHT];
    SpotLight spotLight[MAX_SPOT_LIGHT];
    float4x4 mViewProjInv;  // �r���[�v���W�F�N�V�����s��̋t�s��
    float3 eyePos;          // �J�����̎��_
    int numPointLight;      // �|�C���g���C�g�̐��B    
    float3 ambientLight;    // ����
    int numSpotLight;       // �X�|�b�g���C�g�̐��B
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
    float iblIntencity;     // IBL�̋��x�B
    int isIBL;              // IBL���s���B
    int isEnableRaytracing; // ���C�g�����L���B
};
#endif // _PBRLIGHTING_STRUCT_H_