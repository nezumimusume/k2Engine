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

Texture2D<float4> sceneTexture : register(t0);     //FXAAをかけるレンダーターゲットのシェーダーリソース。
sampler Sampler : register(s0);     //サンプラー。

//FXAA関係。


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
	//近傍4テクセルをフェッチ。
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
	
	
	//近傍テクセルの輝度の差を調べる。
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
		//輝度の差がしきい値以下だったので、このピクセルはアンチをかけない。
		 return rgbyM;
	}
    
/*--------------------------------------------------------------------------*/
	//輝度の差を利用して、ギザギザが発生している可能性の高いテクセルをフェッチする。
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
	//ブレンドブレンド。
    float4 rgbyA = rgbyN1 + rgbyP1;
    float4 rgbyB = ((rgbyN2 + rgbyP2) * 0.25) + (rgbyA * 0.25);
/*--------------------------------------------------------------------------*/
    int twoTap = (rgbyB.y < lumaMin) || (rgbyB.y > lumaMax);
    
    if(twoTap){
		//まだ輝度の差が大きいので、再度ブレンド。
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
    // ガンマ補正。
    color.xyz =  pow(max( color.xyz, 0.0001f ), 1.0f/2.2f);
    return color;
}