//////////////////////////////////////////////////////////////////////////////
// �t�H���[�h�����_�����O�Ŏg�p����郂�f���V�F�[�_�[��SRV_UAV���W�X�^�ݒ�
//////////////////////////////////////////////////////////////////////////////

#ifndef _MODEL_SRV_UAV_REGISTER_H_
#define _MODEL_SRV_UAV_REGISTER_H_

#include "Shadowing_const.h"

Texture2D<float4> albedoTexture : register(t0);                 // �A���x�h
Texture2D<float4> normalTexture : register(t1);                 // �@��
Texture2D<float4> metallicShadowSmoothTexture : register(t2);   // ���^���b�N�A�V���h�E�A�X���[�X�e�N�X�`���Br�ɋ����x�Ag�ɉe�p�����[�^�Aa�Ɋ��炩���B
TextureCube<float4> g_skyCubeMap : register(t11);               // �X�J�C�L���[�u
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t12);  //�V���h�E�}�b�v�B

#endif // _MODEL_SRV_UAV_REGISTER_H_