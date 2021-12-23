/*!
 * @brief	�g�[���}�b�v�B
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
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

//�g�[���}�b�v�̋��ʒ萔�o�b�t�@�B
cbuffer cbTonemapCommon : register(b1){
	float deltaTime;
	float middleGray;
    int currentAvgTexNo;
}

/*!
 * @brief	���_�V�F�[�_�[�B
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
// RGB����HSV��V(�P�x)�����߂�
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
// �P�x�̑ΐ����ς����߂�B
////////////////////////////////////////////////////////

static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);
Texture2D<float4> sceneTexture : register(t0);	//�V�[���e�N�X�`���B
sampler Sampler : register(s0);

static const int    MAX_SAMPLES            = 16;    // Maximum texture grabs

/*!
 * @brief	�萔�o�b�t�@�B
 */
cbuffer cbCalcLuminanceLog : register(b0){
	float4 g_avSampleOffsets[MAX_SAMPLES];
};
/*!
 *@brief	�P�x�̑ΐ����ς����߂�B
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
// �P�x�̕��ς����߂�B
////////////////////////////////////////////////////////
/*!
 *@brief	���ϋP�x�v�Z�s�N�Z���V�F�[�_�[�B
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
// �w���֐����g�p���ĕ��ϋP�x�����߂�
/////////////////////////////////////////////////////////
/*!
 *@brief	�w���֐����g�p���ĕ��ϋP�x�����߂�s�N�Z���V�F�[�_�[�B
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
// ���Ï���
/////////////////////////////////////////////////////////

Texture2D<float4> lumAvgTexture : register(t0);		        //���ϋP�x
Texture2D<float4> lastLumAvgTextureArray[2] : register(t1);	//�P�t���[���O�̕��ϋP�x�̔z��

/*!
 *@brief	���Ï����̂��߂̕��ϋP�x�̓K��������s�N�Z���V�F�[�_�[�B
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
 *@brief	���ϋP�x����g�[���}�b�v���s���s�N�Z���V�F�[�_�[�B
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
    // ���邳���g�[���}�b�v����B
    hsv.z = ( middleGray / ( max(fAvgLum, 0.001f ))) * hsv.z;
    hsv.z = ACESFilm(hsv.z);

    // HSV��RGB�ɕϊ����ďo��
    float4 color;
    color.xyz = Hsv2Rgb(hsv);
    color.w= 1.0f;
    
	return color;
}