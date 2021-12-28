/*!
 * @brief	スプライトシェーダー。
 */

cbuffer cb : register(b0){
	float4x4 mvp;		// モデルビュープロジェクション行列
	float4 mulColor;	// 乗算カラー。ピクセルシェーダーの出力結果に乗算されます。
};
struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> colorTexture : register(t0);
sampler Sampler : register(s0);

PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = mul( mvp, In.pos );
	psIn.uv = In.uv;
	return psIn;
}
float4 PSMain( PSInput In ) : SV_Target0
{
	return colorTexture.Sample(Sampler, In.uv) * mulColor;
}
float4 PSMainGamma( PSInput In ) : SV_Target0
{
	float4 color = colorTexture.Sample(Sampler, In.uv) * mulColor;
	color.xyz =  pow(max( color.xyz, 0.0001f ), 1.0f/2.2f);
	return color;
}