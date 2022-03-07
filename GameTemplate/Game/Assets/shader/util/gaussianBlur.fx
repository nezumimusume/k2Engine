/*!
 * @brief �u���[��
 */

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

/*!
 * @brief ���_�V�F�[�_�[
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}
Texture2D<float4> mainRenderTargetTexture : register(t0); // ���C�������_�����O�^�[�Q�b�g�̃e�N�X�`��
sampler Sampler : register(s0);

/////////////////////////////////////////////////////////
// �u���[
/////////////////////////////////////////////////////////
struct PS_BlurInput
{
    float4 pos : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float4 tex4 : TEXCOORD4;
    float4 tex5 : TEXCOORD5;
    float4 tex6 : TEXCOORD6;
    float4 tex7 : TEXCOORD7;
};

Texture2D<float4> sceneTexture : register(t0); // �V�[���e�N�X�`��

/*!
 * @brief �u���[�p�̒萔�o�b�t�@�[
 */
cbuffer CBBlur : register(b1)
{
    float4 weight[2]; // �d��
}

/*!
 * @brief ���u���[���_�V�F�[�_�[
 */
PS_BlurInput VSXBlur(VSInput In)
{
    // step-13 ���u���[�p�̒��_�V�F�[�_�[������
    PS_BlurInput Out;

    // ���W�ϊ�
    Out.pos = mul(mvp, In.pos);

    // �e�N�X�`���T�C�Y���擾
    float2 texSize;
    float level;
    sceneTexture.GetDimensions(0, texSize.x, texSize.y, level);

    // ��e�N�Z����UV���W���L�^
    float2 tex = In.uv;

    // ��e�N�Z������U���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex0.xy = float2(1.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex1.xy = float2(3.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex2.xy = float2(5.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex3.xy = float2(7.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex4.xy = float2(9.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex5.xy = float2(11.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex6.xy = float2(13.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex7.xy = float2(15.0f / texSize.x, 0.0f);

    // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    Out.tex0.zw = Out.tex0.xy * -1.0f;
    Out.tex1.zw = Out.tex1.xy * -1.0f;
    Out.tex2.zw = Out.tex2.xy * -1.0f;
    Out.tex3.zw = Out.tex3.xy * -1.0f;
    Out.tex4.zw = Out.tex4.xy * -1.0f;
    Out.tex5.zw = Out.tex5.xy * -1.0f;
    Out.tex6.zw = Out.tex6.xy * -1.0f;
    Out.tex7.zw = Out.tex7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    Out.tex0 += float4(tex, tex);
    Out.tex1 += float4(tex, tex);
    Out.tex2 += float4(tex, tex);
    Out.tex3 += float4(tex, tex);
    Out.tex4 += float4(tex, tex);
    Out.tex5 += float4(tex, tex);
    Out.tex6 += float4(tex, tex);
    Out.tex7 += float4(tex, tex);

    return Out;
}

/*!
 * @brief Y�u���[���_�V�F�[�_�[
 */
PS_BlurInput VSYBlur(VSInput In)
{
    // step-14 Y�u���[�p�̒��_�V�F�[�_�[������

    PS_BlurInput Out;

    // ���W�ϊ�
    Out.pos = mul(mvp, In.pos);

    // �e�N�X�`���T�C�Y���擾
    float2 texSize;
    float level;
    sceneTexture.GetDimensions(0, texSize.x, texSize.y, level);

    // ��e�N�Z����UV���W���L�^
    float2 tex = In.uv;

    // ��e�N�Z������V���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex0.xy = float2(0.0f, 1.0f / texSize.y);

    // ��e�N�Z������V���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex1.xy = float2(0.0f, 3.0f / texSize.y);

    // ��e�N�Z������V���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex2.xy = float2(0.0f, 5.0f / texSize.y);

    // ��e�N�Z������V���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex3.xy = float2(0.0f, 7.0f / texSize.y);

    // ��e�N�Z������V���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex4.xy = float2(0.0f, 9.0f / texSize.y);

    // ��e�N�Z������V���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex5.xy = float2(0.0f, 11.0f / texSize.y);

    // ��e�N�Z������V���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex6.xy = float2(0.0f, 13.0f / texSize.y);

    // ��e�N�Z������V���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    Out.tex7.xy = float2(0.0f, 15.0f / texSize.y);

    // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    Out.tex0.zw = Out.tex0.xy * -1.0f;
    Out.tex1.zw = Out.tex1.xy * -1.0f;
    Out.tex2.zw = Out.tex2.xy * -1.0f;
    Out.tex3.zw = Out.tex3.xy * -1.0f;
    Out.tex4.zw = Out.tex4.xy * -1.0f;
    Out.tex5.zw = Out.tex5.xy * -1.0f;
    Out.tex6.zw = Out.tex6.xy * -1.0f;
    Out.tex7.zw = Out.tex7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    Out.tex0 += float4(tex, tex);
    Out.tex1 += float4(tex, tex);
    Out.tex2 += float4(tex, tex);
    Out.tex3 += float4(tex, tex);
    Out.tex4 += float4(tex, tex);
    Out.tex5 += float4(tex, tex);
    Out.tex6 += float4(tex, tex);
    Out.tex7 += float4(tex, tex);
    return Out;
}

/*!
 * @brief �u���[�s�N�Z���V�F�[�_�[
 */
float4 PSBlur(PS_BlurInput In) : SV_Target0
{
    // step-15 X,Y�u���[�p�̃s�N�Z���V�F�[�_�[������
    float4 Color;

    // ��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    Color  = weight[0].x * sceneTexture.Sample(Sampler, In.tex0.xy);
    Color += weight[0].y * sceneTexture.Sample(Sampler, In.tex1.xy);
    Color += weight[0].z * sceneTexture.Sample(Sampler, In.tex2.xy);
    Color += weight[0].w * sceneTexture.Sample(Sampler, In.tex3.xy);
    Color += weight[1].x * sceneTexture.Sample(Sampler, In.tex4.xy);
    Color += weight[1].y * sceneTexture.Sample(Sampler, In.tex5.xy);
    Color += weight[1].z * sceneTexture.Sample(Sampler, In.tex6.xy);
    Color += weight[1].w * sceneTexture.Sample(Sampler, In.tex7.xy);

    // ��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    Color += weight[0].x * sceneTexture.Sample(Sampler, In.tex0.zw);
    Color += weight[0].y * sceneTexture.Sample(Sampler, In.tex1.zw);
    Color += weight[0].z * sceneTexture.Sample(Sampler, In.tex2.zw);
    Color += weight[0].w * sceneTexture.Sample(Sampler, In.tex3.zw);
    Color += weight[1].x * sceneTexture.Sample(Sampler, In.tex4.zw);
    Color += weight[1].y * sceneTexture.Sample(Sampler, In.tex5.zw);
    Color += weight[1].z * sceneTexture.Sample(Sampler, In.tex6.zw);
    Color += weight[1].w * sceneTexture.Sample(Sampler, In.tex7.zw);
    return float4(Color.xyz, 1.0f);
}
/*!
 * @brief �u���[�s�N�Z���V�F�[�_�[(���`�����l���ɂ��u���[��������B)
 */
float4 PSBlur_Alpha(PS_BlurInput In) : SV_Target0
{
    // step-15 X,Y�u���[�p�̃s�N�Z���V�F�[�_�[������
    float4 Color;

    // ��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    Color  = weight[0].x * sceneTexture.Sample(Sampler, In.tex0.xy);
    Color += weight[0].y * sceneTexture.Sample(Sampler, In.tex1.xy);
    Color += weight[0].z * sceneTexture.Sample(Sampler, In.tex2.xy);
    Color += weight[0].w * sceneTexture.Sample(Sampler, In.tex3.xy);
    Color += weight[1].x * sceneTexture.Sample(Sampler, In.tex4.xy);
    Color += weight[1].y * sceneTexture.Sample(Sampler, In.tex5.xy);
    Color += weight[1].z * sceneTexture.Sample(Sampler, In.tex6.xy);
    Color += weight[1].w * sceneTexture.Sample(Sampler, In.tex7.xy);

    // ��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    Color += weight[0].x * sceneTexture.Sample(Sampler, In.tex0.zw);
    Color += weight[0].y * sceneTexture.Sample(Sampler, In.tex1.zw);
    Color += weight[0].z * sceneTexture.Sample(Sampler, In.tex2.zw);
    Color += weight[0].w * sceneTexture.Sample(Sampler, In.tex3.zw);
    Color += weight[1].x * sceneTexture.Sample(Sampler, In.tex4.zw);
    Color += weight[1].y * sceneTexture.Sample(Sampler, In.tex5.zw);
    Color += weight[1].z * sceneTexture.Sample(Sampler, In.tex6.zw);
    Color += weight[1].w * sceneTexture.Sample(Sampler, In.tex7.zw);

    return Color;
}
