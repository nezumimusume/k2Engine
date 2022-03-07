///////////////////////////////////////
// ZPrepass
///////////////////////////////////////


///////////////////////////////////////
// �\���́B
///////////////////////////////////////


// �s�N�Z���V�F�[�_�[�ւ̓���
struct SPSIn
{
    float4 pos : SV_POSITION;       //���W�B
    float3 depth : TEXCOORD0;       //�[�x�l�Bx�ɂ̓v���W�F�N�V������ԁAy�ɂ̓J������Ԃł̐��K�����ꂽZ�l�Az�ɂ̓J������Ԃł�Z�l
};


///////////////////////////////////////
// ���_�V�F�[�_�[�̋��ʏ������C���N���[�h����B
///////////////////////////////////////
#include "../ModelVSCommon.h"


///////////////////////////////////////
// �֐�
///////////////////////////////////////

// ���f���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal, uniform bool isUsePreComputedVertexBuffer)
{
    SPSIn psIn;
    // ���_���W�����[���h���W�n�ɕϊ�����B
    psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal, isUsePreComputedVertexBuffer);
    psIn.pos = mul(mView, psIn.pos);        // ���[���h���W�n����J�������W�n�ɕϊ�
    psIn.depth.z = psIn.pos.z;
    psIn.pos = mul(mProj, psIn.pos);        // �J�������W�n����X�N���[�����W�n�ɕϊ�
    psIn.depth.x = psIn.pos.z / psIn.pos.w;
    psIn.depth.y = saturate( psIn.pos.w / 1000.0f );
    
    return psIn;
}

// ���f���p�̃s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
float4 PSMain(SPSIn psIn) : SV_Target0
{
    return float4( psIn.pos.z, psIn.depth.y, psIn.depth.z, 1.0f );
}


