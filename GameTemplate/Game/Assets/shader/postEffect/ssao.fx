/// <summary>
/// ssao�B
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //�r���[�v���W�F�N�V�����s��B
    float4 mulColor;
};

cbuffer ssaoBuffer : register(b1) {
    float4x4 view;//�r���[�s��B
    float4x4 proj;//�v���W�F�N�V�����s��B
    float4x4 invproj;//�t�v���W�F�N�V�����s��B
};

struct VSInput {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> zPrepassTexture : register(t0);     //�[�x�l�����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�B
Texture2D<float4> ssaoTexture : register(t0);     //ssao�����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�B
Texture2D<float4> normalTexture : register(t1);       //�@�������_�\�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�B
Texture2D<float4> worldTexture : register(t2);       //���[���h���W�����_�\�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�B

sampler Sampler : register(s0);     //�T���v���[�B

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//uv���W���痐���𐶐��B
float random(float2 uv) 
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f)))*43758.5453f);
}

float4 PSMain(PSInput input) : SV_Target0
{

	const float SAMPLING_RATIO = 2.0;       // �T���v���_�̐��̔�r�W��
  
	//�[�x�l�}�b�v����[�x�l�Q�b�g�B
	float dp = zPrepassTexture.Sample(Sampler, input.uv).x;

	float w, h, miplevels;
	zPrepassTexture.GetDimensions(0, w, h, miplevels);
	float dx = 1.0f / w;
	float dy = 1.0f / h;

	//SSAO�B
	//uv�l����A���̍��W�𕜌��B
	//float4 respos = mul(invproj, float4(input.uv*float2(2, -2) + float2(-1, 1), dp, 1));
	float4 respos = mul(view, worldTexture.Sample(Sampler, input.uv));
	respos.xyz = respos.xyz / respos.w;

	float div = 0.0f;
	float ao = 0.0f;
	//�@���x�N�g����@���}�b�v����擾���āA���̖@���̒l�ɖ߂��Ă���B
	float3 norm = normalize((normalTexture .Sample(Sampler, input.uv).xyz * 2) - 1);
	//�v�Z����񐔁B
	const int trycnt = 30;
	//�����̔��a�B
	const float radius = 30.5f;

	float sumDepth = 0.0f;

	float4 rpos;
	float realDepth;

	int count = 0;
	if (dp < 1.0f) {
		for (int i = 0; i < trycnt; ++i) {
			//��������K���ȃx�N�g�����쐬����B
			float rnd1 = random(float2(i*dx, i*dy)) * 2 - 1;
			float rnd2 = random(float2(rnd1, i*dy)) * 2 - 1;
			float rnd3 = random(float2(rnd2, rnd1)) * 2 - 1;
			float3 omega = normalize(float3(rnd1,rnd2,rnd3));
			//�K���ȃx�N�g���Ɩ@���̓��ς����B
			float dt = dot(norm, omega);
			//sign�œ��ς̕���(+�A-)���擾����B
			float sgn = sign(dt);
			//-�̏ꍇ��+�ɕϊ�����B
			omega *= sgn;
			
			//���ς̕�����-�Ȃ�+�ɂ���B
			dt *= sgn;
			//cos���̑��a�����߂����̂ŁA���Z����B
			div += dt;
			

			//���߂����̍��W�ɓK���ȃx�N�g�������Z���B
			//�r���[�s��ƃv���W�F�N�V�����s����A�X�N���[����̍��W�����߂�B
			rpos = mul(proj, float4(respos.xyz + omega * radius, 1));
			//rpos = mul(proj, mul(view,float4(respos.xyz + omega * radius, 1)));
			//w�Ŋ���B
			rpos.xyz /= rpos.w;

			//rpos.xy *= float2(0.5f, -0.5f);
			//rpos.xy += 0.5f;
			//UV���W�ɕϊ��B
			//rpos.xy = (rpos.xy + float2(1, -1))* float2(0.5f, -0.5f);
			//�X�N���[�����W��UV���W�ɕϊ��B
			rpos.xy *= float2(0.5f, -0.5f);
			rpos.xy += 0.5f;

			if (rpos.x >= 0.0f && rpos.x <= 1.0f
				&& rpos.y >= 0.0f && rpos.y <= 1.0f)
			{
				//���ۂ̐[�x�l�������Ă���B
				realDepth = zPrepassTexture.Sample(Sampler, rpos.xy).x;

				//�[�x�l�}�b�v������ۂ̐[�x�l���r���āA�Օ�����Ă�����1.0f*cos�������Z����B
				//ao += step(realDepth, rpos.z) * dt;
				if (rpos.z < realDepth)
				{
					count++;
				}

				sumDepth += abs(realDepth - rpos.z);
			}
		}
		//cos���̑��a(�S�Ă��Օ�����Ă������̒l)�Ŋ���B
		// /= div;
		sumDepth /= trycnt;
	
	}

	// �Օ�����Ȃ��|�C���g�̐�������Օ��W�������߂�
	float a = clamp(float(count) * SAMPLING_RATIO / float(trycnt), 0.0, 1.0);


	float brightNess = 0.0f;
	//�T���v�����O�����[�x�l-�v�Z�����[�x�l�̕��ρB
	//brightNess = sumDepth;
	brightNess = 1.0f - ao;
	//�v�Z�����[�x�l�B
	//brightNess = rpos.z;
	//�T���v�����O�����[�x�l�B
	//brightNess = realDepth;
	return float4(a, a, a, 1.0f);
}

//SSAO(??Z?p????x????????????)
/*float PSMain(PSInput input) : SV_Target0
{
	float dp = zPrepassTexture.Sample(Sampler, input.uv).x;//?????UV??[?x

	float w, h, miplevels;
	zPrepassTexture.GetDimensions(0, w, h, miplevels);
	float dx = 1.0f / w;
	float dy = 1.0f / h;

	//??????W???????
	float4 respos = mul(invproj, float4(input.uv*float2(2, -2) + float2(-1, 1), dp, 1));
	respos.xyz = respos.xyz / respos.w;
	float div = 0.0f;
	float ao = 0.0f;
	float3 norm = normalize((normalTexture.Sample(Sampler, input.uv).xyz * 2.0f) - 1.0f);
	const int trycnt = 36;
	const float radius = 0.5f;

    float depth = 0.0f;
	
	if (dp < 1.0f) {
		for (int i = 0; i < trycnt; ++i) {

			float rnd1 = random(float2(i*dx, i*dy)) * 2 - 1;
			float rnd2 = random(float2(rnd1, i*dy)) * 12 - 1;
			float rnd3 = random(float2(rnd2, rnd1)) * 2 - 1;
			float3 omega = normalize(float3(rnd1, rnd2, rnd3));

			omega = normalize(omega);
			float dt = dot(norm, omega);
			float sgn = sign(dt);
			omega *= sgn;
            dt *= sgn;
            div += dt;
			float4 rpos = mul(proj, mul(view,float4(respos.xyz + omega* radius, 1)));
            //float4 rpos = mul(proj, mul(view,float4(respos.xyz, 1)));
			rpos.xyz /= rpos.w;
            //�v���W�F�N�V�����s�񂩂�UV���W�ɕϊ�����B
            float2 rposUV = rpos.xy;
            rposUV *= float2(0.5f,-0.5f);
            rposUV += 0.5f;
            rposUV = (rpos.xy + float2(1, -1))*float2(0.5, -0.5);
			ao += step(zPrepassTexture.Sample(Sampler, rposUV).x, rpos.z) * dt;
            depth = zPrepassTexture.Sample(Sampler, rposUV).x;
		}
		ao /= (float)trycnt;
	}
   
	return 1.0f - ao;
    //return depth;
}*/