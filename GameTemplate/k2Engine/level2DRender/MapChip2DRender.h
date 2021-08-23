#pragma once

namespace nsK2Engine {
	struct Level2DObjectData;
	/// <summary>
	/// �}�b�v�`�b�v2D�B
	/// </summary>
	class MapChip2DRender : public Noncopyable
	{
	public:
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="objData">���x��2D�f�[�^�B</param>
		void Init(Level2DObjectData* objData);
		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update()
		{
			m_spriteRender.Update();
		}
		/// <summary>
		/// �X�v���C�g��`�悷��B
		/// </summary>
		/// <param name="renderContext">�����_�[�R���e�L�X�g�B</param>
		void Draw(RenderContext& renderContext)
		{
			m_spriteRender.Draw(renderContext);
		}
	private:
		SpriteRender m_spriteRender;			//�X�v���C�g�B
	};
}

