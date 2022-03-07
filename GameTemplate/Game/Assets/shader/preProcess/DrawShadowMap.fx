/*!
 * @brief �V���h�E�}�b�v�`��p�̃V�F�[�_�[
 */

// �s�N�Z���V�F�[�_�[�ւ̓���
struct SPSIn
{
    float4 pos : SV_POSITION;   // �X�N���[����Ԃł̃s�N�Z���̍��W
    float2 depth : TEXCOORD1;   // ���C�g��Ԃł̐[�x���
};

static const int INFINITY = 40.0f; 

///////////////////////////////////////
// ���_�V�F�[�_�[�̋��ʏ������C���N���[�h����B
///////////////////////////////////////

#include "../ModelVSCommon.h"


///////////////////////////////////////////////////
// �O���[�o���ϐ�
///////////////////////////////////////////////////

// ���f���p�̒��_�V�F�[�_�[�̃G���g���[�|�C���g
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal, uniform bool isUsePreComputedVertexBuffer)
{
    SPSIn psIn;

    psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal, isUsePreComputedVertexBuffer);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    return psIn;
}
/// <summary>
/// �V���h�E�}�b�v�`��p�̃s�N�Z���V�F�[�_�[
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
    float depth = psIn.pos.z ;
    float pos = exp(INFINITY * depth);
    return float4(pos, pos*pos, 0.0f, 1.0f);
}
