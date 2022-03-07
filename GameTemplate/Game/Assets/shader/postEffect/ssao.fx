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
Texture2D<float4> worldTexture : register(t2);       //ワールド座標レンダ―ターゲットのシェーダーリソース。

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

float4 PSMain(PSInput input) : SV_Target0
{

	const float SAMPLING_RATIO = 2.0;       // サンプル点の数の比較係数
  
	//深度値マップから深度値ゲット。
	float dp = zPrepassTexture.Sample(Sampler, input.uv).x;

	float w, h, miplevels;
	zPrepassTexture.GetDimensions(0, w, h, miplevels);
	float dx = 1.0f / w;
	float dy = 1.0f / h;

	//SSAO。
	//uv値から、元の座標を復元。
	//float4 respos = mul(invproj, float4(input.uv*float2(2, -2) + float2(-1, 1), dp, 1));
	float4 respos = mul(view, worldTexture.Sample(Sampler, input.uv));
	respos.xyz = respos.xyz / respos.w;

	float div = 0.0f;
	float ao = 0.0f;
	//法線ベクトルを法線マップから取得して、元の法線の値に戻している。
	float3 norm = normalize((normalTexture .Sample(Sampler, input.uv).xyz * 2) - 1);
	//計算する回数。
	const int trycnt = 30;
	//半球の半径。
	const float radius = 30.5f;

	float sumDepth = 0.0f;

	float4 rpos;
	float realDepth;

	int count = 0;
	if (dp < 1.0f) {
		for (int i = 0; i < trycnt; ++i) {
			//乱数から適当なベクトルを作成する。
			float rnd1 = random(float2(i*dx, i*dy)) * 2 - 1;
			float rnd2 = random(float2(rnd1, i*dy)) * 2 - 1;
			float rnd3 = random(float2(rnd2, rnd1)) * 2 - 1;
			float3 omega = normalize(float3(rnd1,rnd2,rnd3));
			//適当なベクトルと法線の内積を取る。
			float dt = dot(norm, omega);
			//signで内積の符号(+、-)を取得する。
			float sgn = sign(dt);
			//-の場合は+に変換する。
			omega *= sgn;
			
			//内積の符号を-なら+にする。
			dt *= sgn;
			//cosΘの総和を求めたいので、加算する。
			div += dt;
			

			//求めた元の座標に適当なベクトルを加算し。
			//ビュー行列とプロジェクション行列より、スクリーン上の座標を求める。
			rpos = mul(proj, float4(respos.xyz + omega * radius, 1));
			//rpos = mul(proj, mul(view,float4(respos.xyz + omega * radius, 1)));
			//wで割る。
			rpos.xyz /= rpos.w;

			//rpos.xy *= float2(0.5f, -0.5f);
			//rpos.xy += 0.5f;
			//UV座標に変換。
			//rpos.xy = (rpos.xy + float2(1, -1))* float2(0.5f, -0.5f);
			//スクリーン座標をUV座標に変換。
			rpos.xy *= float2(0.5f, -0.5f);
			rpos.xy += 0.5f;

			if (rpos.x >= 0.0f && rpos.x <= 1.0f
				&& rpos.y >= 0.0f && rpos.y <= 1.0f)
			{
				//実際の深度値を持ってくる。
				realDepth = zPrepassTexture.Sample(Sampler, rpos.xy).x;

				//深度値マップから実際の深度値を比較して、遮蔽されていたら1.0f*cosΘを加算する。
				//ao += step(realDepth, rpos.z) * dt;
				if (rpos.z < realDepth)
				{
					count++;
				}

				sumDepth += abs(realDepth - rpos.z);
			}
		}
		//cosΘの総和(全てが遮蔽されていた時の値)で割る。
		// /= div;
		sumDepth /= trycnt;
	
	}

	// 遮蔽されないポイントの数から環境遮蔽係数を求める
	float a = clamp(float(count) * SAMPLING_RATIO / float(trycnt), 0.0, 1.0);


	float brightNess = 0.0f;
	//サンプリングした深度値-計算した深度値の平均。
	//brightNess = sumDepth;
	brightNess = 1.0f - ao;
	//計算した深度値。
	//brightNess = rpos.z;
	//サンプリングした深度値。
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