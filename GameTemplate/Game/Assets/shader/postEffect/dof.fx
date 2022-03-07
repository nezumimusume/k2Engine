/*!
 * @brief ��ʊE�[�x
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

//step-11 �{�P�摜�Ɛ[�x�e�N�X�`���ɃA�N�Z�X���邽�߂̕ϐ���ǉ��B
Texture2D<float4> bokeTexture : register(t0);  // �{�P�摜
Texture2D<float4> depthTexture : register(t1); // �[�x�e�N�X�`��

sampler Sampler : register(s0);

/////////////////////////////////////////////////////////
// �{�P�摜�������ݗp�B
/////////////////////////////////////////////////////////

float4 PSMain(PSInput In) : SV_Target0
{
    // step-12 �{�P�摜�`�����ݗp�̃s�N�Z���V�F�[�_�[�������B
    // �J������Ԃł̐[�x�l���T���v�����O�B
    float depth = depthTexture.Sample( Sampler, In.uv ).z;
    // �J������Ԃł̐[�x�l��200�ȉ��Ȃ�s�N�Z���L�� 
    //      -> �{�P�摜��`�����܂Ȃ��B
    clip( depth - 500.0f);
    // �{�P�摜���T���v�����O�B
    float4 boke = bokeTexture.Sample( Sampler, In.uv );
    // �[�x�l����s�����x���v�Z����B
    // �[�x�l200����{�P���n�܂�A�[�x�l500�ōő�̃{�P��ɂȂ�B
    //  -> �܂�A�[�x�l500�ŕs�����x��1�ɂȂ�B
    boke.a = min( 1.0f, ( depth - 500.0f ) / 1000.0f );
    // �{�P�摜���o�́B
    return boke;
}
