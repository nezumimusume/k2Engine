#pragma once

#include "CascadeShadowMapMatrix.h"


namespace nsK2Engine {
    class IRenderer;
    /// <summary>
    /// �V���h�E�}�b�v�ւ̕`�揈��
    /// </summary>
    class ShadowMapRender : public Noncopyable
    {
    public:
        /// <summary>
        /// ������
        /// </summary>
        /// <param name="isSoftShadow">
        /// true�̏ꍇ�A�V���h�E�}�b�v�@�ɂ��A�e�������\�t�g�V���h�E�ɂȂ�܂��B
        /// �n�[�h�V���h�E�ɂ������ꍇ�́Afalse���w�肵�Ă��������B
        /// </param>
        void Init(bool isSoftShadow);

        /// <summary>
        /// �`��
        /// </summary>
        /// <param name="rc">�����_�����O�R���e�L�X�g</param>
        /// <param name="sceneMaxPosition">�Q�[���V�[���̍ő���W</param>
        /// <param name="sceneMinPosition">�Q�[���V�[���̍ŏ����W</param>
        void Render(
            RenderContext& rc,
            int ligNo,
            Vector3& lightDirection,
            std::vector< IRenderer* >& renderObjects,
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition
        );
        /// <summary>
        /// �V���h�E�}�b�v���擾
        /// </summary>
        /// <param name="areaNo">�G���A�ԍ�</param>
        /// <returns></returns>
        Texture& GetShadowMap(int areaNo)
        {
            if (m_isSoftShadow) {
                return m_blur[areaNo].GetBokeTexture();
            }
            return m_shadowMaps[areaNo].GetRenderTargetTexture();
        }
        /// <summary>
        /// ���C�g�r���[�v���W�F�N�V�����s����擾
        /// </summary>
        /// <returns></returns>
        const Matrix& GetLVPMatrix(int areaNo) const
        {
            return m_cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(areaNo);
        }
        /// <summary>
        /// �J�X�P�[�h�V���h�E�̃G���A����ݒ�B
        /// </summary>
        /// <remark>
        /// �Q�[���J�����̋ߕ��ʂ��牓���ʂ܂ł̃G���A�́A��%�͈̔͂��ߋ����p�̍��𑜓x�̃V���h�E�}�b�v�ɁA
        /// ��%�𒆋����p�̃V���h�E�}�b�v�ɁA��%���������p�̃V���h�E�}�b�v�ɕ`�悷�邩���w�肵�����́B
        /// �Ⴆ�΁AnearArea  = 0.1(10%)�AmiddleArea = 0.3(30%), farArea = 0.8(80%)�Ǝw�肳��Ă�����A
        /// �J��������10%�̃G���A�͋ߋ����p�̃V���h�E�}�b�v�ɕ`��A
        /// �J��������10%�`30%�̃G���A�͒������p�̃V���h�E�}�b�v�ɕ`��A
        /// �J��������30%�`80%�̃G���A�͉������p�̃V���h�E�}�b�v�ɕ`��ƂȂ�B
        /// �e�̕i���̒����Ɏg���Ă��������B
        /// </remark>
        /// <param name="nearArea">�߉e�G���A���B</param>
        void SetCascadeNearAreaRates(float nearArea, float middleArea, float farArea)
        {
            // �������G���A�͈̔͂��ߋ����G���A�͈̔͂�菬�����Ȃ��Ă����
            // �v�Z��s�s�����N����̂ŁA�␳�B
            middleArea = max(nearArea + 0.01f, middleArea);
            // ���������G���A�͈̔͂��������G���A�͈̔͂�菬�����Ȃ��Ă����
            // �v�Z��s�s�����N����̂ŁA�␳�B
            farArea = max(middleArea + 0.01f, farArea);
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_NEAR] = nearArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_MIDDLE] = middleArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_FAR] = farArea;
        }
    private:
        CascadeShadowMapMatrix m_cascadeShadowMapMatrix;    // �J�X�P�[�h�V���h�E�}�b�v�̍s��������I�u�W�F�N�g
        RenderTarget m_shadowMaps[NUM_SHADOW_MAP];          // �V���h�E�}�b�v
        std::vector< IRenderer* > m_renderers;              // �V���h�E�}�b�v�ւ̃����_���[�̔z��B
        float m_cascadeAreaRateArray[NUM_SHADOW_MAP] = { 0.05f,0.3f, 1.0f };
        GaussianBlur m_blur[NUM_SHADOW_MAP];                // �V���h�E�}�b�v�Ƀu���[�������鏈���B�\�t�g�V���h�E���s���ۂɎg���܂��B
        bool m_isSoftShadow = false;                        // �\�t�g�V���h�E�H
    };
}