#pragma once
#include "MapChip2DRender.h"

namespace nsK2Engine {

	struct CaslData;
	/// <summary>
	/// 2D�̃��x���f�[�^�B
	/// </summary>
	struct Level2DObjectData {
		Vector3 position = Vector3::Zero;		//���W�B
		Quaternion rotation = Quaternion::Identity;	//��]�B
		Vector3 scale = Vector3::One;			//�g�嗦�B
		int width = 0;		//�����B
		int height = 0;		//�c���B
		const char* name;	//���O�B
		const char* ddsFilePath;		//.dds�t�@�C���̃t�@�C���p�X�B
		Vector2 pivot = Sprite::DEFAULT_PIVOT;	//�s�{�b�g�B
		/// <summary>
		/// �����œn�����I�u�W�F�N�g���̃I�u�W�F�N�g�𒲂ׂ�B
		/// </summary>
		/// <param name="objName">���ׂ閼�O�B</param>
		/// <returns>���O�������ꍇ��true��Ԃ��܂��B</returns>
		bool EqualObjectName(const char* objName) const
		{
			return strcmp(objName, name) == 0;
		}
		/// <summary>
		/// ���O���O����v���邩���ׂ�B
		/// </summary>
		/// <param name="n">���ׂ閼�O�B</param>
		/// <returns>���O�������ꍇ��true��Ԃ��܂��B</returns>
		bool ForwardMatchName(const char* n) const
		{
			auto len = strlen(n);
			auto namelen = strlen(name);
			if (len > namelen) {
				//���O�������B�s��v�B
				return false;
			}
			return strncmp(n, name, len) == 0;
		}
	};
	/// <summary>
	/// ���x��2D
	/// </summary>
	class Level2DRender : public Noncopyable {
	private:
		using MapChip2DRenderPtr = std::unique_ptr<MapChip2DRender>;
	public:
		/// <summary>
		/// ���x�����������B
		/// </summary>
		/// <param name="filePath">casl�t�@�C���̃t�@�C���p�X�B</param>
		/// <param name="hookFunc">�I�u�W�F�N�g���쐬���鎞�̏������t�b�N���邽�߂̊֐��I�u�W�F�N�g�B</param>
		void Init(const char* filePath, std::function<bool(Level2DObjectData& objData)> hookFunc);
		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update() const;
		/// <summary>
		/// �}�b�v�`�b�v2D�̕`�揈���B
		/// </summary>
		/// <param name="renderContext">�����_�[�R���e�L�X�g�B</param>
		void Draw(RenderContext& renderContext) const;
	private:
		/// <summary>
		/// �}�b�v�`�b�v2D�����_�[��ǉ�����B
		/// </summary>
		/// <param name="objData">���x��2D�f�[�^</param>
		void AddMapChip2DRender(Level2DObjectData* objData);
	private:
		std::vector<MapChip2DRenderPtr> m_mapChip2DRenderPtrList;		//�}�b�v�`�b�v2D�����_�[�B
	};
}