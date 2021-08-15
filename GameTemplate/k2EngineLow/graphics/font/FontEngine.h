#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �t�H���g�G���W���B
	/// </summary>
	/// <remarks>
	/// DirectXTK�̃t�H���g�\�������̏������`������s���N���X�B
	/// BeginDraw�Ńt�H���g�̕`��J�n�ADraw�ŕ`�惊�N�G�X�g�AEnd�Ń��N�G�X�g���ꊇ�����B
	/// �t�H���g�̕`��͂܂Ƃ߂čs���قǍ����ɂȂ�܂��B
	/// </remarks>
	/// <example>
	/// Font font;
	/// font.BeginDraw( renderContext );
	/// font.Draw("Hello world", pos, color, rot, scale, pivot);
	/// font.EndDraw( renderContext );
	/// </example>
	class FontEngine : public Noncopyable {

	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~FontEngine();
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
		/// <summary>
		/// �`��J�n�B
		/// </summary>
		void BeginDraw(RenderContext& rc);
		/// <summary>
		/// �������`��B
		/// </summary>
		/// <param name="position">���W</param>
		/// <param name="color">�J���[</param>
		/// <param name="rotation">��]</param>
		/// <param name="scale">�g��</param>
		/// <param name="pivot">�s�{�b�g</param>
		void Draw(
			const wchar_t* text,
			const Vector2& position,
			const Vector4& color,
			float rotation,
			float scale,
			Vector2 pivot
		);
		/// <summary>
		/// �`��I���B
		/// </summary>
		void EndDraw(RenderContext& rc);
	private:
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;	//�X�v���C�g�o�b�`�B
		std::unique_ptr<DirectX::SpriteFont>	m_spriteFont;	//�X�v���C�g�t�H���g�B
		ID3D12DescriptorHeap* m_srvDescriptorHeap = nullptr;	//SRV�̃f�B�X�N���v�^�q�[�v�B
	};
}