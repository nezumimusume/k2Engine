#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �t�H���g
	/// </summary>
	class Font : public Noncopyable {
	public:
		/// <summary>
		/// �`��J�n
		/// </summary>
		/// <param name="rc">�����_�����O�R���e�L�X�g�B</param>
		void Begin(RenderContext& rc);
		
		/// <summary>
		/// �`��I��
		/// </summary>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void End(RenderContext& rc);
		/// <summary>
		/// �`��
		/// </summary>
		/// <param name="text"></param>
		/// <param name="position"></param>
		/// <param name="color"></param>
		/// <param name="rotation"></param>
		/// <param name="scale"></param>
		/// <param name="pivot"></param>
		void Draw(
			wchar_t const* text,
			const Vector2& position,
			const Vector4& color,
			float rotation,
			float scale,
			Vector2 pivot
		);
		/*!
			*@brief	�e�̃p�����[�^��ݒ�B
			*@param[in]	isDrawShadow		�e��`���H
			*@param[in]	shadowOffset		�e��`���Ƃ��̃s�N�Z���̃I�t�Z�b�g�ʁB
			*@param[in]	shadowColor			�e�̐F�B
			*/
		
		/// <summary>
		/// �e�̃p�����[�^��ݒ�
		/// </summary>
		/// <param name="isDrawShadow">�e��`�悷��</param>
		/// <param name="shadowOffset">�e�I�t�Z�b�g</param>
		/// <param name="shadowColor">�e�J���[</param>
		void SetShadowParam(bool isDrawShadow, float shadowOffset, const Vector4& shadowColor)
		{
			m_isDrawShadow = isDrawShadow;
			m_shadowOffset = shadowOffset;
			m_shadowColor = shadowColor;
		}
	private:
		DirectX::SpriteBatch* m_spriteBatch = nullptr;		//�@�X�v���C�g�o�b�`�B
		DirectX::SpriteFont* m_spriteFont = nullptr;		//�@�X�v���C�g�t�H���g�B
		bool m_isDrawShadow = false;						//�@�e�������H
		float m_shadowOffset = 0.0f;						//�@�e�������Ƃ��̃s�N�Z���̃I�t�Z�b�g�ʁB
		Vector4 m_shadowColor;								//�@�e�̐F�B
		Matrix m_scaleMat;
	};
}