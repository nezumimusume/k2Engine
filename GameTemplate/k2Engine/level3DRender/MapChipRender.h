/*!
 *@brief	�}�b�v�`�b�v
 */

#pragma once




struct LevelObjectData;

/// <summary>
/// �}�b�v�`�b�v�����_�[�B
/// </summary>
class MapChipRender : public Noncopyable
{
public:
	MapChipRender(const LevelObjectData& objData, const char* filePath);
	~MapChipRender()
	{
	}
	/// <summary>
	/// ���f����`��B
	/// </summary>
	/// <param name="rc">�����_�[�R���e�L�X�g�B</param>
	void Draw(RenderContext& rc);
	/// <summary>
	/// ���f�������_���[���擾�B
	/// </summary>
	/// <returns></returns>
	ModelRender& GetSkinModelRender()
	{
		return m_modelRender;
	}
private:
	ModelRender			m_modelRender;					//���f�������_�[�B
	PhysicsStaticObject m_physicsStaticObject;			//�ÓI�����I�u�W�F�N�g�B
};
