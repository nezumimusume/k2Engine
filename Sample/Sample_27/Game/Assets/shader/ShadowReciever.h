// シャドウレシーバー関係の処理をまとめる。


///////////////////////////////////////
// サンプラステート。
///////////////////////////////////////
sampler Sampler : register(s0);


float CalcShadowRate(int ligNo, float3 worldPos, int isSoftShadow)
{
    float shadow = 0.0f;
    for(int cascadeIndex = 0; cascadeIndex < NUM_SHADOW_MAP; cascadeIndex++)
    {
        float4 posInLVP = mul( mlvp[ligNo][cascadeIndex], float4( worldPos, 1.0f ));
        float2 shadowMapUV = posInLVP.xy / posInLVP.w;
        float zInLVP = posInLVP.z / posInLVP.w;
        shadowMapUV *= float2(0.5f, -0.5f);
        shadowMapUV += 0.5f;
        // シャドウマップUVが範囲内か判定
        if(shadowMapUV.x >= 0.0f && shadowMapUV.x <= 1.0f
            && shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f)
        {
            // シャドウマップから値をサンプリング
            float3 shadowValue = g_shadowMap[ligNo][cascadeIndex].Sample(Sampler, shadowMapUV).xyz;

            // まずこのピクセルが遮蔽されているか調べる
            if(zInLVP >= shadowValue.r + 0.001f)
            {
                if( isSoftShadow ){
                    // ソフトシャドウ。
                    // 遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
                    float depth_sq = shadowValue.x * shadowValue.x;
                    // このグループの分散具合を求める
                    // 分散が大きいほど、varianceの数値は大きくなる
                    float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);
                    // このピクセルのライトから見た深度値とシャドウマップの平均の深度値の差を求める
                    float md = zInLVP - shadowValue.x;
                    // 光が届く確率を求める
                    float lit_factor = variance / (variance + md * md);
                    // 光が届く確率から影になる確率を計算する。
                    shadow = 1.0f - pow( lit_factor, 5.0f ) ;
                }else{
                    // ハードシャドウ。
                    shadow = 1.0f;
                }
            }
            break;
        }
    }
    return shadow;
}
