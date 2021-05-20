/// <summary>
/// tonemap。
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //ビュープロジェクション行列。
    float4 mulColor;
};

cbuffer TonemapBuffer : register(b1) {
    float scaler;
    float gammaCorrection;
}

struct VSInput {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> lumaTexture : register(t0);     //レンダーターゲットのシェーダーリソース。
Texture2D<float4> albedoTexture : register(t1);       //メインレンダ―ターゲットのシェーダーリソース。

static const float KEY_VALUE = 0.18f;

sampler Sampler : register(s0);     //サンプラー。

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//rgbから輝度を計算する。
float ToneMapLuma(float3 rgb) {
    float luma = dot(rgb, float3(0.2125f, 0.7154f, 0.0721f));
    luma += 0.01f;
    return  luma;
}

float4 ExtractLumaPSMain(PSInput In) : SV_Target0
{
    float4 color = lumaTexture.Sample(Sampler,In.uv);
    float luma = ToneMapLuma(color.xyz);

    return float4(luma,luma,luma,1.0f);
}

float4 PSMain(PSInput In) : SV_Target0
{
    float luma = lumaTexture.Sample(Sampler,In.uv).x;
    return float4(luma,luma,luma,1.0f);
}

float4 FinalPSMain(PSInput In) : SV_Target0
{
    float averageLuma = lumaTexture.Sample(Sampler,In.uv).x;
    float4 color = albedoTexture.Sample(Sampler,In.uv);
    float luma = ToneMapLuma(color.xyz);
 
    float scale = (KEY_VALUE / averageLuma) / luma;
    scale *= scaler;
    scale = scale / (1 + scale);
    scale = pow(scale, (1.0f / gammaCorrection));
    color.xyz *= scale;
    
    return color;
}