/*!
 * @brief	トーンマップ。
 */

struct VSInput{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};

//トーンマップの共通定数バッファ。
cbuffer cbTonemapCommon : register(b1){
	float deltaTime;
	float middleGray;
    int currentAvgTexNo;
}

/*!
 * @brief	頂点シェーダー。
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

float3 Rgb2Hsv(float3 rgb)
{
    float3 hsv;
    
    // RGB 2 HSV
    float fMax = max(rgb.r, max(rgb.g, rgb.b));
    float fMin = min(rgb.r, min(rgb.g, rgb.b));
    float delta = fMax - fMin;

	hsv.z = fMax; // v
	if (fMax != 0.0){
	    hsv.y = delta / fMax;//s
	}else{
	    hsv.y = 0.0;//s
    }
	
//	if (hsv.y == 0.0) {
//		hsv.x = NO_HUE; // h
//	} else {
      if ( rgb.r == fMax ){
          hsv.x =     (rgb.g - rgb.b) / delta;// h
      }else if (rgb.g == fMax){
          hsv.x = 2 + (rgb.b - rgb.r) / delta;// h
      }else{
          hsv.x = 4 + (rgb.r - rgb.g) / delta;// h
      }
      hsv.x /= 6.0;
      if (hsv.x < 0) hsv.x += 1.0;
//  }

    return hsv;
}
// RGBからHSVのV(輝度)を求める
float Rgb2V( float3 rgb)
{
    return max(rgb.r, max(rgb.g, rgb.b));
}
float3 Hsv2Rgb(float3 hsv)
{
    float3 ret;
    // HSV 2 RGB
    if ( hsv.y == 0 ){ /* Grayscale */
        ret.r = ret.g = ret.b = hsv.z;// v
    } else {
        if (1.0 <= hsv.x) hsv.x -= 1.0;
        hsv.x *= 6.0;
        float i = floor (hsv.x);
        float f = hsv.x - i;
        float aa = hsv.z * (1 - hsv.y);
        float bb = hsv.z * (1 - (hsv.y * f));
        float cc = hsv.z * (1 - (hsv.y * (1 - f)));
        if( i < 1 ){
	        ret.r = hsv.z; ret.g = cc;    ret.b = aa;
        }else if( i < 2 ){
	    	ret.r = bb;    ret.g = hsv.z; ret.b = aa;
        }else if( i < 3 ){
    		ret.r = aa;    ret.g = hsv.z; ret.b = cc;
        }else if( i < 4 ){
    		ret.r = aa;    ret.g = bb;    ret.b = hsv.z;
        }else if( i < 5 ){
    		ret.r = cc;    ret.g = aa;    ret.b = hsv.z;
        }else{
    		ret.r = hsv.z; ret.g = aa;    ret.b = bb;
        }
    }
	return ret;
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

        vSample = max( sceneTexture.Sample(Sampler, In.uv+g_avSampleOffsets[iSample].xy), 0.001f );
        float v = Rgb2V( vSample );
        fLogLumSum += log(v);
    }
    
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

Texture2D<float4> lumAvgTexture : register(t0);		        //平均輝度
Texture2D<float4> lastLumAvgTextureArray[2] : register(t1);	//１フレーム前の平均輝度の配列

/*!
 *@brief	明暗順応のための平均輝度の適合させるピクセルシェーダー。
 */
float4 PSCalcAdaptedLuminance( PSInput In ) : SV_Target0
{
	float fAdaptedLum;
    
    if( currentAvgTexNo == 0){
        fAdaptedLum = lastLumAvgTextureArray[1].Sample(Sampler, float2( 0.5f, 0.5f));
    }else{
        fAdaptedLum = lastLumAvgTextureArray[0].Sample(Sampler, float2( 0.5f, 0.5f));
    } 
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


float ACESFilm(float x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

/*!
 *@brief	平均輝度からトーンマップを行うピクセルシェーダー。
 */
float4 PSFinal( PSInput In) : SV_Target0
{
    
	float4 vSample = sceneTexture.Sample(Sampler, In.uv );
    float3 hsv = Rgb2Hsv(vSample.xyz);

	float fAvgLum = 0.0f;
    if( currentAvgTexNo == 0){
        fAvgLum = lastLumAvgTextureArray[0].Sample(Sampler, float2( 0.5f, 0.5f)).r;
    }else{
        fAvgLum = lastLumAvgTextureArray[1].Sample(Sampler, float2( 0.5f, 0.5f)).r;
    }
    // 明るさをトーンマップする。
    hsv.z = ( middleGray / ( max(fAvgLum, 0.001f ))) * hsv.z;
    hsv.z = ACESFilm(hsv.z);

    // HSVをRGBに変換して出力
    float4 color;
    color.xyz = Hsv2Rgb(hsv);
    color.w= 1.0f;
    
	return color;
}