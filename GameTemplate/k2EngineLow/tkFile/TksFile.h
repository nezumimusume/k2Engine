/// <summary>
/// tks�t�@�C��
/// </summary>
/// <remarks>
/// tks�t�@�C���͓Ǝ��̃X�P���g���t�H�[�}�b�g�ł��B
/// ���̃N���X�𗘗p���邱�Ƃ�tks�t�@�C�����������Ƃ��ł��܂��B
/// </remarks>
#pragma once

namespace nsK2EngineLow {

	class TksFile : public Noncopyable {
	public:
		
		/// <summary>
		/// �{�[���B
		/// </summary>
		struct SBone {
			std::unique_ptr<char[]> name;	//���̖��O�B
			int parentNo;					//�e�̔ԍ��B
			float bindPose[4][3];			//�o�C���h�|�[�Y�B
			float invBindPose[4][3];		//�o�C���h�|�[�Y�̋t���B
			int no;							//�{�[���̔ԍ��B
		};
		/// <summary>
		/// TKS�t�@�C�������[�h����B
		/// </summary>
		/// <param name="filePath"></param>
		bool Load(const char* filePath);
		/// <summary>
		/// �{�[���ɑ΂��ăN�G�����s���B
		/// </summary>
		/// <param name="query">�N�G���֐�</param>
		void QueryBone(std::function<void(SBone& bone)> query)
		{
			for (auto& bone : m_bones) {
				query(bone);
			}
		}
	private:
		int m_numBone = 0;			//���̐��B
		std::vector<SBone> m_bones;	//���̃��X�g�B
	};
}