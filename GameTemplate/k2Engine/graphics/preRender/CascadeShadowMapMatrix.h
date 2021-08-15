#pragma once

namespace nsK2Engine {

    /// <summary>
    /// �J�X�P�[�h�V���h�E�}�b�v�@�Ŏg�p�����s��N���X
    /// </summary>
    class CascadeShadowMapMatrix : public Noncopyable
    {
    public:
        /// <summary>
        /// ���C�g�r���[�v���W�F�N�V�����N���b�v�s����v�Z����
        /// </summary>
        /// <param name="lightDirection">���C�g�̕���</param>
        void CalcLightViewProjectionCropMatrix(
            Vector3 lightDirection,
            float cascadeAreaRateTbl[NUM_SHADOW_MAP],
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition
        );
        /// <summary>
        /// �v�Z���ꂽ���C�g�r���[�v���W�F�N�V�����N���b�v�s����擾
        /// </summary>
        /// <param name="shadowMapNo">�V���h�E�}�b�v�̔ԍ�</param>
        /// <returns></returns>
        const Matrix& GetLightViewProjectionCropMatrix(int shadowMapNo) const
        {
            return m_lvpcMatrix[shadowMapNo];
        }
    private:
        Matrix m_lvpcMatrix[NUM_SHADOW_MAP];	//���C�g�r���[�N���b�v�s��
        float m_near[NUM_SHADOW_MAP];
        float m_far[NUM_SHADOW_MAP];
    };
}