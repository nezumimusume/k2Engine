/// <summary>
/// ssao。
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //ビュープロジェクション行列。
    float4 mulColor;
};

cbuffer ssaoBuffer : register(b1) {
    float4x4 view;//ビュー行列。
    float4x4 proj;//プロジェクション行列。
    float4x4 invproj;//逆プロジェクション行列。
    float4x4 invView;//逆ビュー行列。
};

struct VSInput {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> zPrepassTexture : register(t0);     //深度値レンダーターゲットのシェーダーリソース。
Texture2D<float4> ssaoTexture : register(t0);     //ssaoレンダーターゲットのシェーダーリソース。
Texture2D<float4> normalTexture : register(t1);       //法線レンダ―ターゲットのシェーダーリソース。

sampler Sampler : register(s0);     //サンプラー。

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//uv座標から乱数を生成。
float random(float2 uv) 
{
	return frac(sin(dot(uv, float2(12.9898f, 78.233f)))*43758.5453f);
}
float PSMain(PSInput input) : SV_Target0
{
    float4x4 mat;
	mat._11_12_13_14 = float4(1.0f, 2.0f, 3.0f, 4.0f);

	float dp = zPrepassTexture.Sample(Sampler, input.uv).x;//���݂�UV�̐[�x

	float w, h, miplevels;
	zPrepassTexture.GetDimensions(0, w, h, miplevels);
	float dx = 1.0f / w;
	float dy = 1.0f / h;

	//SSAO
	//���̍��W�𕜌�����
	float4 respos = mul(invproj, float4(input.uv*float2(2, -2) + float2(-1, 1), dp, 1));
	respos.xyz = respos.xyz / respos.w;
	float div = 0.0f;
	float ao = 0.0f;
	float3 norm = normalize((normalTexture .Sample(Sampler, input.uv).xyz * 2) - 1);
	const int trycnt = 40;
	const float radius = 30.5f;
	if (dp < 1.0f) {
		for (int i = 0; i < trycnt; ++i) {
			float rnd1 = random(float2(i*dx, i*dy)) * 2 - 1;
			float rnd2 = random(float2(rnd1, i*dy)) * 2 - 1;
			float rnd3 = random(float2(rnd2, rnd1)) * 2 - 1;
			float3 omega = normalize(float3(rnd1,rnd2,rnd3));
			omega = normalize(omega);
			//�����̌��ʖ@���̔��Α��Ɍ����Ă��甽�]����
			float dt = dot(norm, omega);
			float sgn = sign(dt);
			omega *= sign(dt);
			//���ʂ̍��W���Ăюˉe�ϊ�����
			float4 rpos = mul(proj, float4(respos.xyz + omega * radius, 1));
			rpos.xyz /= rpos.w;
			dt *= sgn;
			div += dt;
			//�v�Z���ʂ����݂̏ꏊ�̐[�x��艜�ɓ����Ă�Ȃ�Օ�����Ă���Ƃ������Ȃ̂ŉ��Z
			ao += step(zPrepassTexture.Sample(Sampler, (rpos.xy + float2(1, -1))*float2(0.5f, -0.5f)).x, rpos.z)*dt;
		}
		ao /= div;
	}
	return 1.0f - ao;
}

float4 PSMain(PSInput In) : SV_TARGET0
{
    
}

//SSAO(��Z�p�̖��x�̂ݏ���Ԃ���΂悢)
/*float PSMain(PSInput input) : SV_Target0
{
	float dp = zPrepassTexture.Sample(Sampler, input.uv).x;//���݂�UV�̐[�x

	float w, h, miplevels;
	zPrepassTexture.GetDimensions(0, w, h, miplevels);
	float dx = 1.0f / w;
	float dy = 1.0f / h;

	//���̍��W�𕜌�����
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
            //プロジェクション行列からUV座標に変換する。
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