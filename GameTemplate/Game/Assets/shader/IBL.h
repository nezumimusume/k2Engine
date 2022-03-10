////////////////////////////////////////////////
// IBL関係の処理
////////////////////////////////////////////////

#ifndef _IBL_H_
#define _IBL_H_

#include "Sampler.h"

/// <summary>
/// IBLカラーをスカイキューブマップからサンプリング
/// </summary>
/// <remark>
/// この関数ではサーフェイスの滑らかさを使って、
/// サンプリングするスカイキューブマップのmipmapレベルを計算しています。
/// サーフェイスが滑らかであれば、高解像度のスカイキューブマップ、
/// 滑らかさが低ければ、低解像度のスカイキューブマップからIBLカラーがサンプリングされます。
/// このようにすることで滑らかなサーフェイスであれば、綺麗な映り込みが発生し、
/// 粗いサーフェイスであれば、拡散した映り込みが発生するようになります。
/// </remark>
/// <param name="skyCubeMap">スカイキューブマップ</param>
/// <param name="reflection">反射ベクトル。</param>
/// <param name="smooth">サーフェイスの滑らかさ(0.0～1.0)</param>
/// <param name="iblIntencity">IBLの強度。</param>
float4 SampleIBLColorFromSkyCube( 
    TextureCube<float4> skyCubeMap, 
    float3 reflection, 
    float smooth,
    float iblIntencity
)
{
    int level = lerp(0, 12, 1 - smooth);
    return skyCubeMap.SampleLevel(Sampler, reflection, level) * iblIntencity;
}
/// <summary>
/// IBLカラーをスカイキューブマップからサンプリング
/// </summary>
/// <remark>
/// アルゴリズムは同じなので省略。
/// </remark>
/// <param name="skyCubeMap">スカイキューブマップ</param>
/// <param name="samper">サンプラ</param>
/// <param name="toEye">視点への正規化されたベクトル</param>
/// <param name="normal">サーフェイスの法線</param>
/// <param name="smooth">サーフェイスの滑らかさ(0.0～1.0)</param>
/// <param name="iblIntencity">IBLの強度。</param>
float4 SampleIBLColorFromSkyCube( 
    TextureCube<float4> skyCubeMap,
    float3 toEye, 
    float3 normal, 
    float smooth,
    float iblIntencity
)
{
    // 反射ベクトルを計算する。
    float3 v = reflect(toEye * -1.0f, normal);
    return SampleIBLColorFromSkyCube(skyCubeMap, v, smooth, iblIntencity);
}

#endif // _IBL_H_
