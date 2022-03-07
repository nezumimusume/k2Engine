///////////////////////////////////////
// PBR�x�[�X�̃f�B�t�@�[�h���C�e�B���O��SRV_UAV���W�X�^�ݒ�
///////////////////////////////////////


#ifndef _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_
#define _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_

#include "Shadowing_const.h"

Texture2D<float4> albedoTexture : register(t0);                                         // �A���x�h
Texture2D<float4> normalTexture : register(t1);                                         // �@��
Texture2D<float4> metallicShadowSmoothTexture : register(t2);                           // ���^���b�N�A�V���h�E�A�X���[�X�e�N�X�`���Br�ɋ����x�Ag�ɉe�p�����[�^�Aa�Ɋ��炩���B
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t3);    // �V���h�E�}�b�v�B
TextureCube<float4> g_skyCubeMap : register(t15);                                       // �X�J�C�L���[�u
StructuredBuffer<uint> pointLightListInTile : register(t20);                            // �^�C�����Ƃ̃|�C���g���C�g�̃C���f�b�N�X�̃��X�g
StructuredBuffer<uint> spotLightListInTile : register(t21);                             // �^�C�����Ƃ̃X�|�b�g���C�g�̃C���f�b�N�X�̃��X�g�B
Texture2D<float4> g_reflectionTextureArray[NUM_REFLECTION_TEXTURE] : register(t22);     // ���t���N�V�����e�N�X�`���B


#endif // _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_