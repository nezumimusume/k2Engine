/*!
 * @brief	シンプルなモデルシェーダー。
 */


////////////////////////////////////////////////
// 定数バッファ。
////////////////////////////////////////////////
//モデル用の定数バッファ
cbuffer ModelCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////
//スキニング用の頂点データをひとまとめ。
struct SSkinVSIn{
	int4  Indices  	: BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};
//頂点シェーダーへの入力。
struct SVSIn{
	float4 pos 		: POSITION;		//モデルの頂点座標。
	float2 uv 		: TEXCOORD0;	//UV座標。
	SSkinVSIn skinVert;				//スキン用のデータ。
};
//ピクセルシェーダーへの入力。
struct SPSIn{
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float2 uv 			: TEXCOORD0;	//uv座標。
	float4 posInProj    : TEXCOORD1;
};

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);				//アルベドマップ
StructuredBuffer<float4x4> g_boneMatrix : register(t3);	//ボーン行列。
sampler g_sampler : register(s0);	//サンプラステート。
Texture2D<float4> g_depthTexture : register(t10);
////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

/// <summary>
//スキン行列を計算する。
/// </summary>
float4x4 CalcSkinMatrix(SSkinVSIn skinVert)
{
	float4x4 skinning = 0;	
	float w = 0.0f;
	[unroll]
    for (int i = 0; i < 3; i++)
    {
        skinning += g_boneMatrix[skinVert.Indices[i]] * skinVert.Weights[i];
        w += skinVert.Weights[i];
    }
    
    skinning += g_boneMatrix[skinVert.Indices[3]] * (1.0f - w);
	
    return skinning;
}

/// <summary>
/// 頂点シェーダーのコア関数。
/// </summary>
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;
	float4x4 m;
	if( hasSkin ){
		m = CalcSkinMatrix(vsIn.skinVert);
	}else{
		m = mWorld;
	}
	psIn.pos = mul(m, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	psIn.uv = vsIn.uv;
	psIn.posInProj = psIn.pos;
	psIn.posInProj.xy /= psIn.posInProj.w;
	return psIn;
}

/// <summary>
/// スキンなしメッシュ用の頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn, false);
}
/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSSkinMain( SVSIn vsIn ) 
{
	return VSMainCore(vsIn, true);
}
/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain( SPSIn psIn ) : SV_Target0
{
    // 正規化スクリーン座標系からUV座標系に変換する
	float2 uv = psIn.posInProj.xy * float2(0.5f, -0.5f) + 0.5f;

	// 近傍8テクセルへのUVオフセット
	float2 uvOffset[8] = {
		float2(0.0f,  1.0f / 720.0f), //上
		float2(0.0f, -1.0f / 720.0f), //下
		float2(1.0f / 1280.0f,           0.0f), //右
		float2(-1.0f / 1280.0f,           0.0f), //左
		float2(1.0f / 1280.0f,  1.0f / 720.0f), //右上
		float2(-1.0f / 1280.0f,  1.0f / 720.0f), //左上
		float2(1.0f / 1280.0f, -1.0f / 720.0f), //右下
		float2(-1.0f / 1280.0f, -1.0f / 720.0f)  //左下
	};

	// このピクセルの深度値を取得
	float depth = g_depthTexture.Sample(g_sampler, uv).x;

	// 近傍8テクセルの深度値の平均値を計算する
	float depth2 = 0.0f;
	for (int i = 0; i < 8; i++)
	{
		depth2 += g_depthTexture.Sample(g_sampler, uv + uvOffset[i]).x;
	}
	depth2 /= 8.0f;

	// 自身の深度値と近傍8テクセルの深度値の差を調べる
	if (abs(depth - depth2) > 0.00005f)
	{
		// 深度値が結構違う場合はピクセルカラーを黒にする
		// ->これがエッジカラーとなる
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	// 普通にテクスチャを
	return g_albedo.Sample(g_sampler, psIn.uv);
}
