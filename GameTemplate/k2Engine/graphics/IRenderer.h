#pragma once

namespace nsK2Engine {
	/// <summary>
	/// �����_�[���[�̊��N���X�B
	/// </summary>
	class IRenderer : public Noncopyable {
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
		/// ���_�v�Z�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnComputeVertex( RenderContext& rc )
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
		/// �������I�u�W�F�N�g�`��p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnTlanslucentRender(RenderContext& rc)
		{

		}
		/// <summary>
		/// 2D�`��p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRender2D(RenderContext& rc)
		{

		}
		/// <summary>
		/// �����_���[���`�悷��I�u�W�F�N�g�̍ő���W�ƍŏ����W���擾�B
		/// </summary>
		/// <param name="aabb"></param>
		virtual void GetAABB(Vector3& vMax, Vector3& vMin, bool& isGet)
		{
			vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
			isGet = false;
		}
		/// <summary>
		/// �V���h�E�L���X�^�[�H
		/// </summary>
		/// <returns>true���A���Ă���ƃV���h�E�L���X�^�[</returns>
		virtual bool IsShadowCaster() const
		{
			return false;
		}
	};
}