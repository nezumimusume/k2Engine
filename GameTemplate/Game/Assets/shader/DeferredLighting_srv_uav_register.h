///////////////////////////////////////
// PBRベースのディファードライティングのSRV_UAVレジスタ設定
///////////////////////////////////////


#ifndef _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_
#define _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_

#include "Shadowing_const.h"

Texture2D<float4> albedoTexture : register(t0);                                         // アルベド
Texture2D<float4> normalTexture : register(t1);                                         // 法線
Texture2D<float4> metallicShadowSmoothTexture : register(t2);                           // メタリック、シャドウ、スムーステクスチャ。rに金属度、gに影パラメータ、aに滑らかさ。
Texture2D<float4> g_shadowMap[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP] : register(t3);    // シャドウマップ。
TextureCube<float4> g_skyCubeMap : register(t15);                                       // スカイキューブ
StructuredBuffer<uint> pointLightListInTile : register(t20);                            // タイルごとのポイントライトのインデックスのリスト
StructuredBuffer<uint> spotLightListInTile : register(t21);                             // タイルごとのスポットライトのインデックスのリスト。
Texture2D<float4> g_reflectionTextureArray[NUM_REFLECTION_TEXTURE] : register(t22);     // リフレクションテクスチャ。


#endif // _DEFERREDLIGHTING_SRV_UAV_REGISTER_H_