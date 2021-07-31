#pragma once

/// <summary>
/// �����_�[���[�̊��N���X�B
/// </summary>
class IRenderer : public Noncopyable{
public:
	/// <summary>
	/// �V���h�E�}�b�v�ւ̕`��p�X����Ă΂�鏈���B
	/// </summary>
	/// <param name="rc">�����_�����O�R���e�L�X�g</param>
	/// <param name="ligNo">���C�g�ԍ�</param>
	/// <param name="lvpMatrix">���C�g�r���[�v���W�F�N�V�����s��</param>
	virtual void OnRenderShadowMap(
		RenderContext& rc, 
		int ligNo, 
		int shadowMapNo,
		const Matrix& lvpMatrix
	)
	{

	}
	/// <summary>
	/// ZPrepass����Ă΂�鏈���B
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnZPrepass(RenderContext& rc)
	{

	}
	/// <summary>
	/// G-Buffer�`��p�X����Ă΂�鏈���B
	/// </summary>
	virtual void OnRenderToGBuffer(RenderContext& rc)
	{

	}
	/// <summary>
	/// �t�H���[�h�����_�[�p�X����Ă΂�鏈���B
	/// </summary>
	virtual void OnForwardRender(RenderContext& rc)
	{

	}
	/// <summary>
	/// 2D�`��p�X����Ă΂�鏈���B
	/// </summary>
	/// <param name="rc"></param>
	virtual void OnRender2D(RenderContext& rc)
	{

	}

};