/*!
 * @brief	SSR
 */

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

cbuffer ssrCb : register(b1){
	float4x4 mViewProjInv;				//ビュープロジェクション行列の逆行列。
	float4x4 mViewProj;					//ビュープロジェクション行列。
	float4 cameraPosInWorld;			//ワールド座標系でのカメラの視点。
};


Texture2D<float4> sceneTexture : register(t0); 			// シーンのテクスチャ
Texture2D<float4> depthTexture : register(t1);			//深度テクスチャ。
Texture2D<float4> normalTexture : register(t2);			//法線テクスチャ。
Texture2D<float4> metallicSmoothTexture : register(t3);	//メタリックスムーステクスチャ。

sampler Sampler : register(s0);

/*!
 * @brief	UV座標と深度値からワールド座標を計算する。
 *@param[in]	uv				uv座標
 *@param[in]	zInScreen		スクリーン座標系の深度値
 *@param[in]	mViewProjInv	ビュープロジェクション行列の逆行列。
 */
float3 CalcWorldPosFromUVZ( float2 uv, float zInScreen, float4x4 mViewProjInv )
{
	float3 screenPos;
	screenPos.xy = (uv * float2(2.0f, -2.0f)) + float2( -1.0f, 1.0f);
	screenPos.z = zInScreen;//depthMap.Sample(Sampler, uv).r;
	
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
float GetRandomNumber(float2 texCoord, float Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}
/*!
 * @brief 頂点シェーダー
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}
float4 PSMain(PSInput In) : SV_Target0
{
	//金属度とスムースを取得。
	float4 metallicSmooth = metallicSmoothTexture.Sample(Sampler, In.uv);
	// スムース0.5以上のサーフェイスでのみ映り込みを起こすようにする。
	clip( metallicSmooth.a - 0.5f);
	//ピクセルのワールド座標を計算する。
	float3 worldPos = CalcWorldPosFromUVZ(
		In.uv,
		depthTexture.Sample(Sampler, In.uv).r,
		mViewProjInv
	);

	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv);
	
	//ピクセルの法線を取得。
	float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
	
	//カメラの視点からピクセルに向かうベクトルを計算する。
	float3 toPixelDir = normalize( worldPos - cameraPosInWorld.xyz );
	//反射ベクトルを求める。
	float3 toPixelDirReflect = reflect( toPixelDir, normal );
	float depth = depthTexture.Sample(Sampler, In.uv).r;

	//反射ベクトルを使って、レイマーチングを行う。
	//レイマーチングのイテレーション回数と線分を伸ばす距離は
	//後で定数バッファにするんやで？
	int maxRayNum = 20;
	float3 raystep = 800.0f/ maxRayNum * toPixelDirReflect;
	float maxThickness = 0.001f / maxRayNum;
	for( int step = 1; step < maxRayNum; step++){ //自分自身とぶつかるので、stepは1から進める。
		float3 rayPos = worldPos + step * raystep * lerp( 0.8f, 1.0f, GetRandomNumber(In.uv, 1.0f));
		float4 posInProj = float4(rayPos, 1.0f);
		//ビュープロジェクション空間に変換する。
		posInProj = mul(mViewProj, posInProj);
		posInProj.xyz /= posInProj.w;
		float2 reflectUV = posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
		// レイを進めた先のピクセルのZ値を取得
		float pixelZ = depthTexture.Sample(Sampler, reflectUV).r;
		if ( depth < pixelZ 						// そもそもスタート地点のZ値がぶつかったピクセルより手前。
			&& pixelZ < posInProj.z    				// レイのZ値より、ピクセルのZ値の方が小さい -> つまりレイがぶつかった可能性がある。
			&& posInProj.z - pixelZ < maxThickness	// ぶつかったオブジェクトにある程度の奥行きがあると仮定して、その奥行きの範囲内。
		) {
			// レイがぶつかったと判断する。
			// ここから２分探索で精度を上げる。
			float sign = -1.0; 	// これはレイの進行方向。
			for (int m = 1; m <= 8; ++m) {
				// 0.5をm乗する。これでmが1ならレイステップの1/2、mが2ならレイステップの1/4進むことになる。
				// この2分探索では、ぶつかった地点から、0.5->0.25->0.125->0.0625とステップを進めていき、
				// ぶつかったピクセルの精度を上げていく。
				rayPos += raystep * (sign * pow(0.5, m));
				// 0.5^m進めたレイのスクリーン空間上の座標を求める。
				posInProj = mul(mViewProj, float4(rayPos, 1.0f));
				posInProj.xyz /= posInProj.w;
				reflectUV = posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
				float pixelZ = depthTexture.Sample(Sampler, reflectUV).r;
				// 衝突しなくなったら、レイの進行方向を反転させる。
				// 衝突していたら、そのまま進める。
				sign = posInProj.z - pixelZ > 0 ? -1 : 1;
			}
			float4 reflectColor = sceneTexture.Sample(Sampler, reflectUV);
			// SSRはテクスチャの範囲外をサンプリングすると、正しい結果を得ることはできないので、
			// 範囲外に近づくほど反射が弱くなるような計算を入れる。
			float t = 0.8f;
			t *= pow( 1.0f - min( 1.0f, abs(posInProj.x) ), 0.5f );
			t *= pow( 1.0f - min( 1.0f, abs(posInProj.y) ), 0.5f );
			return float4(reflectColor.xyz, metallicSmooth.a * t);
		
		}
	}
	clip(-1.0f);
	return float4(sceneColor.xyz, 0.0f);
}

Texture2D<float4> reflectTexture : register(t1);	// 反射テクスチャ。
Texture2D<float4> albedoTexture : register(t2);		// アルベドテクスチャ。

//最終合成。
float4 PSFinal(PSInput In) : SV_Target0
{
	
	float4 reflectColor = reflectTexture.Sample(Sampler, In.uv);
	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv);
	float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);

	float3 reflectLig = reflectColor.xyz * reflectColor.a;
	reflectLig *= albedoColor.xyz;
	
	sceneColor.xyz += reflectLig;
	
	return sceneColor;

}
