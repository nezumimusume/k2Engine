/////////////////////////////////////////////////////
// PBRライティング関係の構造体をまとめたヘッダーファイル
/////////////////////////////////////////////////////

#ifndef _PBRLIGHTING_STRUCT_H_
#define _PBRLIGHTING_STRUCT_H_

#include "Shadowing_const.h"

// ディレクションライト構造体。
struct DirectionalLight
{
    float3 direction;   // ライトの方向
    int castShadow;     // 影をキャストする？
    float4 color;       // ライトの色
};
// ポイントライト
struct PointLight
{
    float3 position;        // 座標
    float3 positionInView;  // カメラ空間での座標
    float3 color;           // カラー
    float3 attn;            // 減衰パラメータ。
};
// スポットライト
struct SpotLight
{
    float3 position;        // 座標
    int isUse;              // 使用中フラグ。
    float3 positionInView;  // カメラ空間での座標。
    int no ;                // ライトの番号。
    float3 direction;       // 射出方向。
    float range;            // 影響範囲。
    float3 color;           // ライトのカラー。
    float3 color2;          // 二つ目のカラー。
    float3 color3;          // 三つ目のカラー。
    float3 directionInView; // カメラ空間での射出方向。
    float3 rangePow;        // 距離による光の影響率に累乗するパラメーター。1.0で線形の変化をする。
                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
    float3 angle;           // 射出角度(単位：ラジアン。xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー)。
    float3 anglePow;        // スポットライトとの角度による光の影響率に累乗するパラメータ。1.0で線形に変化する。
                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
};

// 定数バッファで使用するデータをまとめた構造体。
struct LightCB{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
    PointLight pointLight[MAX_POINT_LIGHT];
    SpotLight spotLight[MAX_SPOT_LIGHT];
    float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
    float3 eyePos;          // カメラの視点
    int numPointLight;      // ポイントライトの数。    
    float3 ambientLight;    // 環境光
    int numSpotLight;       // スポットライトの数。
    float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
    float iblIntencity;     // IBLの強度。
    int isIBL;              // IBLを行う。
    int isEnableRaytracing; // レイトレが有効。
};
#endif // _PBRLIGHTING_STRUCT_H_