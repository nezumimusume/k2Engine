///////////////////////////////////////
// PBR�x�[�X�̃f�B�t�@�[�h���C�e�B���O��CAV���W�X�^�ݒ�
///////////////////////////////////////


#ifndef _DEFERREDLIGHTING_CAV_REGISTER_H_
#define _DEFERREDLIGHTING_CAV_REGISTER_H_

//���ʒ萔�o�b�t�@
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
    float4 screenParam;
};

#endif // _DEFERREDLIGHTING_CAV_REGISTER_H_