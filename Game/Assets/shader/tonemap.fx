/// <summary>
/// FXAA。
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //ビュープロジェクション行列。
    float4 mulColor;
};

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
    //float luma = (rgb.y * (0.587f / 0.299f) + rgb.x);
    //luma /= (1.0f * (0.587f / 0.299f) + 1.0f);
    float luma = (rgb.x + rgb.y + rgb.z) / 3.0f;
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
    //return color;
    float scale = KEY_VALUE / averageLuma;
    //color.xyz = (color.xyz * scale) / (float3(1.0f,1.0f,1.0f) + color.xyz * scale) * (1 + (color.xyz * scale / 4.0f));
    //color.xyz = (color.xyz) / (float3(1.0f,1.0f,1.0f) + color.xyz) * (1 + (color.xyz / averageLuma));

    //float r = 1.0f / 1.3f;
    //color.x = pow(color.x,r);
    //color.y = pow(color.y,r);
    //color.z = pow(color.z,r);
    //color.xyz *= (averageLuma);


    float k=log(1.0f/2.8f);
    //float k = scale * 2.0f;
    color.x=1.0f - exp(k*color.x);
    color.y=1.0f - exp(k*color.y);
    color.z=1.0f - exp(k*color.z);

    return color;
    //return float4(1.0f,1.0f,1.0f,1.0f);
}