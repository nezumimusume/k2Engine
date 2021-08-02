/*!
 * @brief	�f�o�b�O���[�h�V�F�[�_�[�B
 */


/////////////////////////////////////////////////////////////
// �萔�o�b�t�@�B
/////////////////////////////////////////////////////////////
/*!
 * @brief	���_�V�F�[�_�[�p�̒萔�o�b�t�@�B
 */
cbuffer VSCb : register(b0){
	float4x4 mVP;
};

/*!
 * @brief	���_�\���́B
 */
struct VSInput
{
	//float4 Position : SV_Position;			//���_���W�B
	//SV_Position�͓��Z�}���e�B�N�X�ƌĂ΂��
	//�Z�}���e�B�N�X�̖��O��SV�����Ȃ���΂Ȃ�ł�����
	float3 position : Position;			//���_���W�B
	float3 color : Color;				//�J���[�B
};

//SV_Position�ŕԂ��p�̍\����
struct PSInput
{
	//���̍\���̂�SV_Position���K�v
	//�ϐ����Ǝ����Ŗ��t�����Z�}���e�B�N�X��VSInput�Ƃ��Ԃ��Ă�����
	float4 position : SV_Position;		//���_���W�B
	float3 color : Color;				//�J���[�B
};

//VSMain�̖߂�l��SV_Position���K�v
PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = mul(mVP, float4(input.position, 1.0f));
	//output.position = mul(mVP,input.position);
	output.color = input.color;
	return output;
}
//PSMain�̖߂�l��SV_Target���K�v
//�߂�l�͐F�ifloat4�j�ł���K�v������
float4 PSMain(PSInput input) : SV_Target
{
	//output.position�͂��̊֐��ɗ���܂łɎg���I����Ă�
	//�ʂɎg���Ă�����

	//float4�ɂ���
	//1�̓A���t�@
    return float4(1.0, 0, 0, 1);
	//return float4(input.color,1);
}