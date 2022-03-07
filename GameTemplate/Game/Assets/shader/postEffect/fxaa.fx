/// <summary>
/// FXAA�B
/// </summary>


cbuffer cb : register(b0) {
    float4x4 mvp;       //�r���[�v���W�F�N�V�����s��B
    float4 mulColor;
};

//�𑜓x��m�邽�߂̒萔�o�b�t�@�\�B
cbuffer CresolutionB : register(b1) {
    float bufferW;  //���̉𑜓x�B
    float bufferH;  //�c�̉𑜓x�B
};


struct VSInput {
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput {
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> sceneTexture : register(t0);     //FXAA�������郌���_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�B
sampler Sampler : register(s0);     //�T���v���[�B

//FXAA�֌W�B


PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

float4 FxaaPixelShader( 
	float2 pos, 
    float4 fxaaConsoleRcpFrameOpt,
    float4 fxaaConsoleRcpFrameOpt2,
    float fxaaQualityEdgeThreshold,
    float fxaaQualityEdgeThresholdMin,
    float fxaaConsoleEdgeSharpness,
    float fxaaConsoleEdgeThreshold,
    float fxaaConsoleEdgeThresholdMin,
    float2 sceneTexSize
)
{
	//�ߖT4�e�N�Z�����t�F�b�`�B
	float4 nTex = float4( 
		pos.x + 0.5f/sceneTexSize.x, 
		pos.y + 0.5f/sceneTexSize.y,
		pos.x - 0.5f/sceneTexSize.x, 
		pos.y - 0.5f/sceneTexSize.y
	);
	float lumaNw = sceneTexture.Sample(Sampler, nTex.xy).y;
	float lumaSw = sceneTexture.Sample(Sampler, nTex.xw).y;
	float lumaNe = sceneTexture.Sample(Sampler, nTex.zy).y;
	float lumaSe = sceneTexture.Sample(Sampler, nTex.zw).y;
	
	float4 rgbyM = sceneTexture.Sample(Sampler, pos.xy);
	float lumaM = rgbyM.y;
	
	
	//�ߖT�e�N�Z���̋P�x�̍��𒲂ׂ�B
	float lumaMaxNwSw = max(lumaNw, lumaSw);
    lumaNe += 1.0/384.0;
    float lumaMinNwSw = min(lumaNw, lumaSw);
/*--------------------------------------------------------------------------*/
    float lumaMaxNeSe = max(lumaNe, lumaSe);
    float lumaMinNeSe = min(lumaNe, lumaSe);
/*--------------------------------------------------------------------------*/
    float lumaMax = max(lumaMaxNeSe, lumaMaxNwSw);
    float lumaMin = min(lumaMinNeSe, lumaMinNwSw);
/*--------------------------------------------------------------------------*/
    float lumaMaxScaled = lumaMax * fxaaConsoleEdgeThreshold;
/*--------------------------------------------------------------------------*/
    float lumaMinM = min(lumaMin, lumaM);
    float lumaMaxScaledClamped = max(fxaaConsoleEdgeThresholdMin, lumaMaxScaled);
    float lumaMaxM = max(lumaMax, lumaM);
    float dirSwMinusNe = lumaSw - lumaNe;
    float lumaMaxSubMinM = lumaMaxM - lumaMinM;
    float dirSeMinusNw = lumaSe - lumaNw;
    if(lumaMaxSubMinM < lumaMaxScaledClamped){
		//�P�x�̍����������l�ȉ��������̂ŁA���̃s�N�Z���̓A���`�������Ȃ��B
		 return rgbyM;
	}
    
/*--------------------------------------------------------------------------*/
	//�P�x�̍��𗘗p���āA�M�U�M�U���������Ă���\���̍����e�N�Z�����t�F�b�`����B
    float2 dir;
    dir.x = dirSwMinusNe + dirSeMinusNw;
    dir.y = dirSwMinusNe - dirSeMinusNw;
   
/*--------------------------------------------------------------------------*/
    float2 dir1 = normalize(dir.xy);
   
    float4 rgbyN1 = sceneTexture.Sample(Sampler, pos.xy - dir1 * fxaaConsoleRcpFrameOpt.zw);
    float4 rgbyP1 = sceneTexture.Sample(Sampler, pos.xy + dir1 * fxaaConsoleRcpFrameOpt.zw);
/*--------------------------------------------------------------------------*/
    float dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * fxaaConsoleEdgeSharpness;
    float2 dir2 = clamp(dir1.xy / dirAbsMinTimesC, -2.0, 2.0);
/*--------------------------------------------------------------------------*/
    float4 rgbyN2 = sceneTexture.Sample(Sampler, pos.xy - dir2 * fxaaConsoleRcpFrameOpt2.zw);
    float4 rgbyP2 = sceneTexture.Sample(Sampler, pos.xy + dir2 * fxaaConsoleRcpFrameOpt2.zw);
    
/*--------------------------------------------------------------------------*/
	//�u�����h�u�����h�B
    float4 rgbyA = rgbyN1 + rgbyP1;
    float4 rgbyB = ((rgbyN2 + rgbyP2) * 0.25) + (rgbyA * 0.25);
/*--------------------------------------------------------------------------*/
    int twoTap = (rgbyB.y < lumaMin) || (rgbyB.y > lumaMax);
    
    if(twoTap){
		//�܂��P�x�̍����傫���̂ŁA�ēx�u�����h�B
		rgbyB.xyz = rgbyA.xyz * 0.5;
	}
 

    return rgbyB;
    
}

float4 PSMain( PSInput In ) : SV_Target0
{
	float2 texSize = float2( bufferW, bufferH);
	float level;
	sceneTexture.GetDimensions( 0, texSize.x, texSize.y, level );
	float4 rcpFrame = float4(0.0f, 0.0f, 1.0f/texSize.x, 1.0f/texSize.y);

    float4 color =   FxaaPixelShader( 
		In.uv,
        rcpFrame,							// float4 fxaaConsoleRcpFrameOpt,
        rcpFrame,							// float4 fxaaConsoleRcpFrameOpt2,
        0.166f,								// FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,							// FxaaFloat fxaaQualityEdgeThresholdMin,
        1.0f,								// FxaaFloat fxaaConsoleEdgeSharpness,
        0.4f,								// FxaaFloat fxaaConsoleEdgeThreshold,
        0.0833f,								// FxaaFloat fxaaConsoleEdgeThresholdMin,
        texSize
	);
    // �K���}�␳�B
    color.xyz =  pow(max( color.xyz, 0.0001f ), 1.0f/2.2f);
    return color;
}