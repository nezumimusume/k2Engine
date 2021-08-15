/*!
 *@brief	�}�b�v�`�b�v
 */

#pragma once

namespace nsK2Engine {
	struct LevelObjectData;

	struct MapChipData
	{
		Vector3				position;				//���W�B
		Vector3				scale;					//�傫���B
		Quaternion			rotation;				//��]�B
		//std::unique_ptr<PhysicsStaticObject> physicsStaticObject;	//�ÓI�����I�u�W�F�N�g�B
	};

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
		/// �}�b�v�`�b�v�f�[�^��ǉ��B
		/// </summary>
		/// <param name="objData">�}�b�v�`�b�v�f�[�^�B</param>
		void AddMapChipData(const LevelObjectData& objData);
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
		/// <summary>
		/// ���f�����X�V�B
		/// </summary>
		void Update();
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
		ModelRender											m_modelRender;					//���f�������_�[�B
		std::vector<MapChipData>							m_mapChipDataVector;			//�}�b�v�`�b�v�f�[�^�̃��X�g�B
		std::unique_ptr<const char*>						m_filePath;						//tkm�t�@�C���p�X�B
		std::vector<std::unique_ptr<PhysicsStaticObject>>	m_physicsStaticObjectPtrVector;	//PhysicsStaticObject�̃X�}�[�g�|�C���^�̔z��B
		int													m_mapChipDataNum;				//�}�b�v�`�b�v�f�[�^�̐��B
	};
}