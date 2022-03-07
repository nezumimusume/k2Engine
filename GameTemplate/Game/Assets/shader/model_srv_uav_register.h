//////////////////////////////////////////////////////////////////////////////
// フォワードレンダリングで使用されるモデルシェーダーのSRV_UAVレジスタ設定
//////////////////////////////////////////////////////////////////////////////

#ifndef _MODEL_SRV_UAV_REGISTER_H_
#define _MODEL_SRV_UAV_REGISTER_H_

#include "Shadowing_const.h"

Texture2D<float4> albedoTexture : register(t0);                 // アルベド
Texture2D<float4> normalTexture : register(t1);                 // 法線
Texture2D<float4> metallicShadowSmoothTexture : register(t2);   // メタリック、シャドウ、スムーステクスチャ。rに金属度、gに影パラメータ、aに滑らかさ。
TextureCube<float4> g_skyCubeMap : register(t11);               // スカイキューブ
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t12);  //シャドウマップ。

#endif // _MODEL_SRV_UAV_REGISTER_H_