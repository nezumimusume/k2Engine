/// <summary>
/// FXAA。
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //ビュープロジェクション行列。
    float4 mulColor;
};

//解像度を知るための定数バッファ―。
cbuffer CresolutionB : register(b1) {
    float bufferW;  //横の解像度。
    float bufferH;  //縦の解像度。
};


struct VSInput {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> albedoTexture : register(t0);     //FXAAをかけるレンダーターゲットのシェーダーリソース。
sampler Sampler : register(s0);     //サンプラー。

//FXAA関係。
static const float FXAA_EDGE_THRESHOLD_MIN = 1.0f / 16.0f;
static const float XAA_EDGE_THRESHOLD = 1.0f / 8.0f;

static const float FXAA_REDUCE_MIN = 1.0f / 128.0f;
static const float FXAA_REDUCE_MUL = 1.0f / 8.0f;
static const float FXAA_SPAN_MAX = 8.0f;

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//rgbから輝度を計算する。
float FxaaLuma(float3 rgb) {
    return rgb.y * (0.587 / 0.299) + rgb.x;
}

//FXAAを計算するよ。
float4 CalcFXAA(float2 uv)
{
    //中央のテクセルカラーを抽出する。
    float4 albedoColor = albedoTexture.Sample(Sampler, uv);

    // 近傍4テクセルへのUVオフセット
    float2 uvOffset[4] = {
        float2(-1.0f / bufferW, 1.0f / bufferH),  //左上
        float2(1.0f / bufferW, 1.0f / bufferH),   //右上
        float2(-1.0f / bufferW, -1.0f / bufferH), //左下
        float2(1.0f / bufferW, -1.0f / bufferH)   //右下
    };
    //近隣テクセルカラーを抽出する。
    float3 albedoNW = albedoTexture.Sample(Sampler, uv + uvOffset[0]).xyz;
    float3 albedoNE = albedoTexture.Sample(Sampler, uv + uvOffset[1]).xyz;
    float3 albedoSW = albedoTexture.Sample(Sampler, uv + uvOffset[2]).xyz;
    float3 albedoSE = albedoTexture.Sample(Sampler, uv + uvOffset[3]).xyz;

    //近隣テクセルの輝度を計算する。
    float lumaM = FxaaLuma(albedoColor.xyz);
    float lumaNW = FxaaLuma(albedoNW);
    float lumaNE = FxaaLuma(albedoNE);
    float lumaSW = FxaaLuma(albedoSW);
    float lumaSE = FxaaLuma(albedoSE);

    //輝度最小値を計算。
    float rangeMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));

    //輝度最大値を計算。
    float rangeMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    //輝度最大値と最小値のコントラストを計算。
    float range = rangeMax - rangeMin;

    //コントラストが輝度最大値*しきい値より低かったら計算を終了するよ。
    //輝度変化が大きかったら、FXAAするよ。
    if (range <
        max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * XAA_EDGE_THRESHOLD)) {
        return albedoColor;
    }


    //輝度エッジに対して垂直なベクトル(方向)を求める。
    float2 dir = float2(0.0f, 0.0f);
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    dir.xy = normalize(dir.xy) * 3.0f;

    //輝度によって、ベクトルの値を変化させる。
    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
        max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
            dir * rcpDirMin));

    //解像度分だけベクトルを割る。
    dir *= float2(1.0f / bufferW, 1.0f / bufferH);

    //方向ベクトルから最終的な色を決定していく。
    float4 rgbA = (1.0f / 2.0f) * (
        albedoTexture.Sample(Sampler, uv + dir * (1.0f / 3.0f - 0.5f)) +
        albedoTexture.Sample(Sampler, uv + dir * (2.0f / 3.0f - 0.5f)));
    float4 rgbB = rgbA * (1.0f / 2.0f) + (1.0f / 4.0f) * (
        albedoTexture.Sample(Sampler, uv + dir * (0.0f / 3.0f - 0.5f)) +
        albedoTexture.Sample(Sampler, uv + dir * (3.0f / 3.0f - 0.5f)));

    float lumaB = FxaaLuma(rgbB.xyz);

    //rgbBの輝度が最小最大から外れていたら。
    if ((lumaB < rangeMin) || (lumaB > rangeMax))
    {
        //rgbAを返す。
        return rgbA;
    }
    else
    {
        return rgbB;
    }
}



float4 PSMain(PSInput In) : SV_Target0
{
    return CalcFXAA(In.uv);
}