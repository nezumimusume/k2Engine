/////////////////////////////////////////////////////
// PBRライティングで使うライト用の定数バッファの中身。
/////////////////////////////////////////////////////

#ifndef _PBRLIGHTING_LIGHTCB_BODY_H_
#define _PBRLIGHTING_LIGHTCB_BODY_H_

DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];
PointLight pointLight[MAX_POINT_LIGHT];
SpotLight spotLight[MAX_SPOT_LIGHT];
float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
float3 eyePos;          // カメラの視点
int numPointLight;      // ポイントライトの数。    
float3 ambientLight;    // 環境光
int numSpotLight;       // スポットライトの数。
float4x4 mlvp[NUM_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];
float iblLuminance;     // IBLの明るさ。
int isIBL;              // IBLを行う。
int isEnableRaytracing; // レイトレが有効。

#endif // _PBRLIGHTING_LIGHTCB_BODY_H_