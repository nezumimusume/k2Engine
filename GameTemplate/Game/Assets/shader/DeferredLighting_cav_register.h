///////////////////////////////////////
// PBRベースのディファードライティングのCAVレジスタ設定
///////////////////////////////////////


#ifndef _DEFERREDLIGHTING_CAV_REGISTER_H_
#define _DEFERREDLIGHTING_CAV_REGISTER_H_

//共通定数バッファ
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
    float4 screenParam;
};

#endif // _DEFERREDLIGHTING_CAV_REGISTER_H_