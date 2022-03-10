////////////////////////////////////////////////
// IBL�֌W�̏���
////////////////////////////////////////////////

#ifndef _IBL_H_
#define _IBL_H_

#include "Sampler.h"

/// <summary>
/// IBL�J���[���X�J�C�L���[�u�}�b�v����T���v�����O
/// </summary>
/// <remark>
/// ���̊֐��ł̓T�[�t�F�C�X�̊��炩�����g���āA
/// �T���v�����O����X�J�C�L���[�u�}�b�v��mipmap���x�����v�Z���Ă��܂��B
/// �T�[�t�F�C�X�����炩�ł���΁A���𑜓x�̃X�J�C�L���[�u�}�b�v�A
/// ���炩�����Ⴏ��΁A��𑜓x�̃X�J�C�L���[�u�}�b�v����IBL�J���[���T���v�����O����܂��B
/// ���̂悤�ɂ��邱�ƂŊ��炩�ȃT�[�t�F�C�X�ł���΁A�Y��ȉf�荞�݂��������A
/// �e���T�[�t�F�C�X�ł���΁A�g�U�����f�荞�݂���������悤�ɂȂ�܂��B
/// </remark>
/// <param name="skyCubeMap">�X�J�C�L���[�u�}�b�v</param>
/// <param name="reflection">���˃x�N�g���B</param>
/// <param name="smooth">�T�[�t�F�C�X�̊��炩��(0.0�`1.0)</param>
/// <param name="iblIntencity">IBL�̋��x�B</param>
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
/// IBL�J���[���X�J�C�L���[�u�}�b�v����T���v�����O
/// </summary>
/// <remark>
/// �A���S���Y���͓����Ȃ̂ŏȗ��B
/// </remark>
/// <param name="skyCubeMap">�X�J�C�L���[�u�}�b�v</param>
/// <param name="samper">�T���v��</param>
/// <param name="toEye">���_�ւ̐��K�����ꂽ�x�N�g��</param>
/// <param name="normal">�T�[�t�F�C�X�̖@��</param>
/// <param name="smooth">�T�[�t�F�C�X�̊��炩��(0.0�`1.0)</param>
/// <param name="iblIntencity">IBL�̋��x�B</param>
float4 SampleIBLColorFromSkyCube( 
    TextureCube<float4> skyCubeMap,
    float3 toEye, 
    float3 normal, 
    float smooth,
    float iblIntencity
)
{
    // ���˃x�N�g�����v�Z����B
    float3 v = reflect(toEye * -1.0f, normal);
    return SampleIBLColorFromSkyCube(skyCubeMap, v, smooth, iblIntencity);
}

#endif // _IBL_H_
