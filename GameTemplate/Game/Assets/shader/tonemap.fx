/*!
 * @brief	トーンマップ。
 */

struct VSInput{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD0;
};
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

//トーンマップの共通定数バッファ。
cbuffer cbTonemapCommon : register(b1){
	float deltaTime;
	float middleGray;
}

/*!
 * @brief	頂点シェーダー。
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = In.pos;
	psIn.uv = In.uv;
	return psIn;
}

////////////////////////////////////////////////////////
// 輝度の対数平均を求める。
////////////////////////////////////////////////////////

static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);
Texture2D<float4> sceneTexture : register(t0);	//シーンテクスチャ。
sampler Sampler : register(s0);

static const int    MAX_SAMPLES            = 16;    // Maximum texture grabs

/*!
 * @brief	定数バッファ。
 */
cbuffer cbCalcLuminanceLog : register(b0){
	float4 g_avSampleOffsets[MAX_SAMPLES];
};
/*!
 *@brief	輝度の対数平均を求める。
 */
float4 PSCalcLuminanceLogAvarage(PSInput In) : SV_Target0
{
	float3 vSample = 0.0f;
    float  fLogLumSum = 0.0f;

    for(int iSample = 0; iSample < 9; iSample++)
    {
        // Compute the sum of log(luminance) throughout the sample points
        vSample = sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy);
        float d = dot(vSample, LUMINANCE_VECTOR)+0.0001f;
        fLogLumSum += log(d);
    }
    
    // Divide the sum to complete the average
    fLogLumSum /= 9;

    return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);
}
////////////////////////////////////////////////////////
// 輝度の平均を求める。
////////////////////////////////////////////////////////
/*!
 *@brief	平均輝度計算ピクセルシェーダー。
 */
float4 PSCalcLuminanceAvarage(PSInput In) : SV_Target0
{
	float fResampleSum = 0.0f; 
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy);
    }
    
    // Divide the sum to complete the average
    fResampleSum /= 16;

    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// 指数関数を使用して平均輝度を求める
/////////////////////////////////////////////////////////
/*!
 *@brief	指数関数を使用して平均輝度を求めるピクセルシェーダー。
 */
float4 PSCalcLuminanceExpAvarage( PSInput In ) : SV_Target0
{
	float fResampleSum = 0.0f;
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample]);
    }
    
    // Divide the sum to complete the average, and perform an exp() to complete
    // the average luminance calculation
    fResampleSum = exp(fResampleSum/16);
    
    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}

/////////////////////////////////////////////////////////
// 明暗順応
/////////////////////////////////////////////////////////

Texture2D<float4> lumAvgTexture : register(t1);		//平均輝度
Texture2D<float4> lastLumAvgTexture : register(t2);	//一フレーム前の平均輝度

/*!
 *@brief	明暗順応のための平均輝度の適合させるピクセルシェーダー。
 */
float4 PSCalcAdaptedLuminance( PSInput In ) : SV_Target0
{
	float fAdaptedLum = lastLumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    float fCurrentLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
    
    // The user's adapted luminance level is simulated by closing the gap between
    // adapted luminance and current luminance by 2% every frame, based on a
    // 30 fps rate. This is not an accurate model of human adaptation, which can
    // take longer than half an hour.
    float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, 60 * deltaTime ) );
    return float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);
}
float4 PSCalcAdaptedLuminanceFirst(PSInput In) : SV_Target0
{
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2(0.5f, 0.5f));
	return float4(fAvgLum, fAvgLum, fAvgLum, 1.0f);
}

/*!
 *@brief	平均輝度からトーンマップを行うピクセルシェーダー。
 */
float4 PSFinal( PSInput In) : SV_Target0
{
	float4 vSample = sceneTexture.Sample(Sampler, In.uv );
	float fAvgLum = lumAvgTexture.Sample(Sampler, float2( 0.5f, 0.5f));
	
	vSample.rgb *= middleGray/max( 0.0001f, fAvgLum );
//	vSample.rgb /= (1.0f+vSample);
	return vSample;
}