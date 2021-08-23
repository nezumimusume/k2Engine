#pragma once

namespace nsK2EngineLow {
	struct CaslData;
	/// <summary>
	/// �}�b�v�`�b�v2D�B
	/// </summary>
	class MapChip2D : public Noncopyable
	{
	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="caslData">Casl�f�[�^�B</param>
		void Init(CaslData* caslData);
		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update()
		{
			m_sprite.Update(m_position, m_rotation, m_scale,m_pivot);
		}
		/// <summary>
		/// �X�v���C�g��`�悷��B
		/// </summary>
		/// <param name="renderContext">�����_�[�R���e�L�X�g�B</param>
		void Draw(RenderContext& renderContext)
		{
			m_sprite.Draw(renderContext);
		}
	private:
		Vector3 m_position;							//���W�B
		Quaternion m_rotation;						//��]�B
		Vector3 m_scale = g_vec3One;				//�傫���B
		Sprite m_sprite;							//�X�v���C�g�B
		Vector2 m_pivot = Sprite::DEFAULT_PIVOT;	//�s�{�b�g�B
	};
}
