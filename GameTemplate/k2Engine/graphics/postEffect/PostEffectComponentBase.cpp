#include "k2EnginePreCompile.h"
#include "PostEffectComponentBase.h"

namespace nsK2Engine {
	void PostEffectComponentBase::Init(
		RenderTarget& mainRenderTarget,
		RenderTarget& zprepassRenderTarget,
		RenderTarget& normalRenderTarget,
		RenderTarget& metallicSmoothRenderTarget,
		RenderTarget& albedoRenderTarget
	)
	{
		OnInit(
			mainRenderTarget,
			zprepassRenderTarget,
			normalRenderTarget,
			metallicSmoothRenderTarget,
			albedoRenderTarget
		);

		if (IsCopyResultTextureToMainRenderTarget()) {
			SpriteInitData initData;
			initData.m_width = mainRenderTarget.GetWidth();
			initData.m_height = mainRenderTarget.GetHeight();
			initData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();
			initData.m_fxFilePath = "Assets/shader/Sprite.fx";
			initData.m_textures[0] = &GetResultTexture();
			m_copyMainRtSprite.Init(initData);
		}
	}

	void PostEffectComponentBase::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		if (!m_isEnable) {
			return;
		}
		OnRender(rc, mainRenderTarget);
		if (IsCopyResultTextureToMainRenderTarget()) {
			// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
			rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
			// �����_�����O�^�[�Q�b�g��ݒ�
			rc.SetRenderTargetAndViewport(mainRenderTarget);
			// �|�X�g�G�t�F�N�g�̌��ʂ����C�������_�����O�^�[�Q�b�g�ɔ��f�B
			m_copyMainRtSprite.Draw(rc);
			// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
			rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
		}
	}
}