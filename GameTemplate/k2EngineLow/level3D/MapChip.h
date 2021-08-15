/// <summary>
/// �}�b�v�`�b�v�B
/// </summary>
#pragma once

namespace nsK2EngineLow {
	struct LevelObjectData;

	/// <summary>
	/// �}�b�v�`�b�v�B
	/// </summary>
	class MapChip : public Noncopyable
	{
	public:
		MapChip(const LevelObjectData& objData, const char* filePath);
		~MapChip()
		{
		}
		/// <summary>
		/// ���f����`��B
		/// </summary>
		/// <param name="rc">�����_�[�R���e�L�X�g�B</param>
		void Draw(RenderContext& rc);
		/// <summary>
		/// ���f�����擾�B
		/// </summary>
		/// <returns></returns>
		Model& GetSkinModel()
		{
			return m_model;
		}
	private:
		Model				m_model;						//���f�������_�[�B
		PhysicsStaticObject m_physicsStaticObject;			//�ÓI�����I�u�W�F�N�g�B
	};
}