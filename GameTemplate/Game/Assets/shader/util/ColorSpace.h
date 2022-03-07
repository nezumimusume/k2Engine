/*!
 *@brief    �F��Ԋ֌W�̃w�b�_�[�t�@�C���B
 */

#ifndef _COLOR_SPACE_H_
#define _COLOR_SPACE_H_
////////////////////////////////////////////////////////
// RGB->HSV, HSV->RGB�ւ̐F��ԕϊ��֘A�̊֐��W�B
////////////////////////////////////////////////////////

/*!
 * @brief RGB�n����HSV�n�ɕϊ�����B
 */
float3 Rgb2Hsv(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
/*!
 * @brief RGB�n����HSV��V(�P�x)�����߂�B
 */
float Rgb2V( float3 rgb)
{
    return max(rgb.r, max(rgb.g, rgb.b));
}
/*!
 * @brief HSV�n����RGB�n�ɕϊ�����B
 */
float3 Hsv2Rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

#endif