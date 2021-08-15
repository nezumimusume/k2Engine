#pragma once

#include "graphics/postEffect/Bloom.h"
#include "graphics/postEffect/Dof.h"
#include "graphics/postEffect/Fxaa.h"
#include "graphics/postEffect/ToneMap.h"
#include "graphics/postEffect/Ssao.h"
#include "graphics/postEffect/Ssr.h"

namespace nsK2Engine {
    /// <summary>
    /// �|�X�g�G�t�F�N�g�B
    /// </summary>
    class PostEffect : public Noncopyable
    {
    public:
        /// <summary>
        /// �������B
        /// </summary>
        /// <param name="mainRenderTarget">�����_�^�[�Q�b�g�B</param>
        /// <param name="zprepassRenderTarget">z�v���p�X�����_�\�^�[�Q�b�g�B</param>
        /// <param name="normalRenderTarget">�@�����������܂�Ă��郌���_�\�^�[�Q�b�g�B</param>
        void Init(
            RenderTarget& mainRenderTarget,
            RenderTarget& zprepassRenderTarget,
            RenderTarget& normalRenderTarget,
            RenderTarget& metallicSmoothRenderTarget,
            RenderTarget& albedoRenderTarget
        );
        /// <summary>
        /// �`��B
        /// </summary>
        /// <param name="rc">�����_�\�R���e�L�X�g�B</param>
        /// <param name="mainRenderTarget">���C�������_�\�^�[�Q�b�g�B</param>
        void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
        /// <summary>
        /// �V�[�����؂�ւ�������Ƃ�ʒm����B
        /// </summary>
        /// <remark>
        /// �V�[���̘A�����𗘗p�����O���t�B�b�N�����̂��߂̊֐��ł��B
        /// ���Ï����ATAA���̏����́A�V�[�����؂�ւ�����Ƃ��ɂ��̊֐����Ăяo���Ȃ���
        /// ���t���[���̊ԁA�\�����������������Ȃ�܂��B
        /// </remark>
        /// <param name="changeSceneTime">�V�[���؂�ւ��ɂ����鎞�ԁB</param>
        void NotifyChangeScene(float changeSceneTime)
        {
            m_tonemap.NotifyChangeScene(changeSceneTime);
        }
    private:
        Bloom m_bloom;	    //�u���[��
        Dof m_dof;		    //��ʊE�[�x
        Fxaa m_fXaa;        //FXAA�B
        ToneMap m_tonemap;  //�g�[���}�b�v�B
        Ssao m_ssao;        //SSAO(�X�N���[���X�y�[�X�A���r�G���g�I�N���[�W����)�B
        Ssr m_ssr;          //SSR(�X�N���[���X�y�[�X���t���N�V����)�B
    };
}
