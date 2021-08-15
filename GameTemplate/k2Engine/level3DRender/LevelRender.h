/*!
 *@brief	���x���B
 */

#pragma once
#include "level3D/TklFile.h"

namespace nsK2Engine {
	class MapChipRender;
	/*!
	* @brief	�I�u�W�F�N�g���B
	*/
	struct LevelObjectData {
		Vector3 position;		//<���W�B
		Quaternion rotation;	//!<��]�B
		Vector3 scale;			//!<�g�嗦�B
		const wchar_t* name;	//!<���O�B
		int number = 0;
		/*!
		* @brief	�����œn�����I�u�W�F�N�g���̃I�u�W�F�N�g�����ׂ�B
		*@param[in]	objName		���ׂ閼�O�B
		*@return	���O�������ꍇ��true��Ԃ��܂��B
		*/
		bool EqualObjectName(const wchar_t* objName)
		{
			return wcscmp(objName, name) == 0;
		}
		/*!
		* @brief	���O���O����v���邩���ׂ�B
		*/
		bool ForwardMatchName(const wchar_t* n)
		{
			auto len = wcslen(n);
			auto namelen = wcslen(name);
			if (len > namelen) {
				//���O�������B�s��v�B
				return false;
			}
			return wcsncmp(n, name, len) == 0;
		}
	};

	/*!
	*@brief	���x���B
	*/
	class LevelRender : public Noncopyable {
	private:
		using MapChipRenderPtr = std::shared_ptr<MapChipRender>;
	public:
		~LevelRender();
		/*!
		 * @brief	���x�����������B
		 *@param[in]	levelDataFilePath		tkl�t�@�C���̃t�@�C���p�X�B
		 *@param[in] hookFunc				�I�u�W�F�N�g���쐬���鎞�̏������t�b�N���邽�߂̊֐��I�u�W�F�N�g�B
		 *   �t�b�N���Ȃ��Ȃ�nullptr���w�肷��΂悢�A
		 * ���̊֐��I�u�W�F�N�g��false��Ԃ��ƁA�I�u�W�F�N�g�̏�񂩂�A
		 * �ÓI�I�u�W�F�N�g��MapChip�N���X�̃C���X�^���X����������܂��B
		 * �I�u�W�F�N�g�̖��O�ȂǂŁA�h�A��W�����v��A�A�C�e���Ȃǂ̓���ȃN���X�̃C���X�^���X�𐶐������Ƃ��ɁA
		 * �f�t�H���g�ō쐬�����MapChip�N���X�̃C���X�^���X���s�v�ȏꍇ��true��Ԃ��Ă��������B
		 * �Ⴆ�΁A�t�b�N�֐��̒��ŁA�n���ꂽ�I�u�W�F�N�g�f�[�^�̖��O�̃��f����`�悷��N���X�̃C���X�^���X��
		 * ���������Ƃ��ɁAfalse��Ԃ��Ă��܂��ƁA�������f������`�悳��邱�ƂɂȂ�܂��B
		 */
		void Init(const char* filePath, std::function<bool(LevelObjectData& objData)> hookFunc);
		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update();
		/// <summary>
		/// ���f����`��B
		/// </summary>
		/// <param name="rc">�����_�[�R���e�L�X�g�B</param>
		void Draw(RenderContext& rc);
	private:
		/// <summary>
		/// �}�b�v�`�b�v���쐬�B
		/// </summary>
		/// <param name="objData">LevelObjectData�B</param>
		void CreateMapChipRender(const LevelObjectData& objData, const char* filePath);
		/// <summary>
		/// Tkl�t�@�C���̍s���ϊ�����B
		/// </summary>
		void MatrixTklToLevel();
	private:
		using BonePtr = std::unique_ptr<Bone>;								//�{�[��Ptr�B
		std::vector<BonePtr> m_bonelist;									//�{�[���̃��X�g�B
		std::unique_ptr<Matrix[]> m_matrixlist;								//�s��̃��X�g�B
		std::map<std::string, MapChipRenderPtr> m_mapChipRenderPtrs;		//�}�b�v�`�b�v�̉ϒ��z��B
		TklFile m_tklFile;													//Tkl�t�@�C���B
	};
}