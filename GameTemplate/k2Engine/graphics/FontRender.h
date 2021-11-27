#pragma once

#include "Graphics/IRenderer.h"

namespace nsK2Engine {
	/// <summary>
	/// �t�H���g�����_�\�B
	/// </summary>
	class FontRender : public IRenderer
	{
	public:
		static const int MAX_TEXT_SIZE = 256;
		~FontRender()
		{

		}
		/// <summary>
		/// �\�����镶����ݒ�B
		/// </summary>
		/// <param name="text">�\�����镶���B</param>
		void SetText(const wchar_t* text)
		{
			swprintf_s(m_text, text);
		}
		/// <summary>
		/// �\�����镶�����擾�B
		/// </summary>
		/// <returns>�\�����镶���B</returns>
		const wchar_t* GetText() const
		{
			return m_text;
		}
		/// <summary>
		/// ���W��ݒ�Bz��0.0f�ŁB
		/// </summary>
		/// <param name="position">���W�B</param>
		void SetPosition(float x, float y, float z)
		{
			SetPosition({ x, y, z });
		}
		/// <summary>
		/// �傫����ݒ�Bz��0.0f�ŁB
		/// </summary>
		/// <param name="position">���W�B</param>
		void SetPosition(const Vector3& position)
		{
			m_position = position;
		}
		/// <summary>
		/// ���W���擾�B
		/// </summary>
		/// <returns>���W�B</returns>
		const Vector3& GetPosition() const
		{
			return m_position;
		}
		/// <summary>
		/// �傫����ݒ�B
		/// </summary>
		/// <param name="scale">�傫���B</param>
		void SetScale(const float scale)
		{
			m_scale = scale;
		}
		/// <summary>
		/// �傫�����擾�B
		/// </summary>
		/// <returns>�傫���B</returns>
		const float GetScale() const
		{
			return m_scale;
		}
		/// <summary>
		/// �F��ݒ�B
		/// </summary>
		/// <param name="color">�F�B</param>
		void SetColor(float r, float g, float b, float a)
		{
			SetColor({ r, g, b, a });
		}
		void SetColor(const Vector4& color)
		{
			m_color = color;
		}
		/// <summary>
		/// �F���擾�B
		/// </summary>
		/// <returns>�F�B</returns>
		const Vector4& GetColor() const
		{
			return m_color;
		}
		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rotation">��]�B</param>
		void SetRotation(const float rotation)
		{
			m_rotation = rotation;
		}
		/// <summary>
		/// ��]���擾�B
		/// </summary>
		/// <returns>��]�B</returns>
		const float GetRotation() const
		{
			return m_rotation;
		}
		/// <summary>
		/// �s�{�b�g��ݒ�B
		/// </summary>
		/// <remark>
		/// <param name="pivot">
		/// �s�{�b�g�B
		/// x = 0.5, y = 0.5�ŉ摜�̒��S����_�B
		/// x = 0.0, y = 0.0�ŉ摜�̍����B
		/// x = 1.0, y = 1.0�ŉ摜�̉E��B
		/// Unity��uGUI�ɏ����B
		/// </param>
		/// </remark>
		/// <param name="x">�s�{�b�g��x���W</param>
		/// <param name="y">�s�{�b�g��y���W</param>
		void SetPivot(float x, float y)
		{
			SetPivot({ x, y });
		}
		/// <summary>
		/// �s�{�b�g��ݒ�B
		/// </summary>
		/// <param name="pivot">
		/// �s�{�b�g�B
		/// x = 0.5, y = 0.5�ŉ摜�̒��S����_�B
		/// x = 0.0, y = 0.0�ŉ摜�̍����B
		/// x = 1.0, y = 1.0�ŉ摜�̉E��B
		/// Unity��uGUI�ɏ����B
		/// </param>
		void SetPivot(const Vector2& pivot)
		{
			m_pivot = pivot;
		}
		/// <summary>
		/// �s�{�b�g���擾�B
		/// </summary>
		/// <returns>�s�{�b�g�B</returns>
		const Vector2& GetPivot() const
		{
			return m_pivot;
		}
		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc">�����_�\�R���e�L�X�g�B</param>
		void Draw(RenderContext& rc);
		/// <summary>
		/// �e�̃p�����[�^��ݒ�B
		/// </summary>
		/// <param name="isDrawShadow">�e��`�悷��H</param>
		/// <param name="shadowOffset">�e��`�悷��Ƃ��̃s�N�Z���̃I�t�Z�b�g�ʁB</param>
		/// <param name="shadowColor">�e�̐F�B</param>
		void SetShadowParam(bool isDrawShadow, float shadowOffset, const Vector4& shadowColor)
		{
			m_font.SetShadowParam(isDrawShadow, shadowOffset, shadowColor);
		}
	private:
		/// <summary>
		/// 2D�`��p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnRender2D(RenderContext& rc) override
		{

			m_font.Begin(rc);
			m_font.Draw(m_text, Vector2(m_position.x, m_position.y), m_color, m_rotation, m_scale, m_pivot);
			m_font.End(rc);
		}
	private:
		Vector3							m_position = Vector3::Zero;			//���W�B	
		float							m_scale = 1.0f;						//�����̑傫���B
		Vector4							m_color = g_vec4White;				//�����̐F�A�f�t�H���g�Ŕ��B
		float							m_rotation = 0.0f;					//��]�B
		Vector2							m_pivot = Sprite::DEFAULT_PIVOT;	//�s�{�b�g�B
		wchar_t							m_text[MAX_TEXT_SIZE];				//�����B
		Font							m_font;								//�t�H���g�B
	};
}
