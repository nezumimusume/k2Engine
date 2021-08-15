#include "k2EngineLowPreCompile.h"
#include "tkFile/TksFile.h"

namespace nsK2EngineLow {
	bool TksFile::Load(const char* filePath)
	{
		auto fp = fopen(filePath, "rb");
		if (fp == nullptr) {
			return false;
		}
		//���̐����擾�B
		fread(&m_numBone, sizeof(m_numBone), 1, fp);
		m_bones.resize(m_numBone);
		for (int i = 0; i < m_numBone; i++) {
			auto& bone = m_bones.at(i);
			size_t nameCount = 0;
			//���̖��O���擾�B
			fread(&nameCount, 1, 1, fp);
			bone.name = std::make_unique<char[]>(nameCount + 1);
			fread(bone.name.get(), nameCount + 1, 1, fp);
			//�e��ID���擾�B
			fread(&bone.parentNo, sizeof(bone.parentNo), 1, fp);
			//�o�C���h�|�[�Y���擾�B
			fread(bone.bindPose, sizeof(bone.bindPose), 1, fp);
			//�o�C���h�|�[�Y�̋t�����擾�B
			fread(bone.invBindPose, sizeof(bone.invBindPose), 1, fp);
			//�{�[���̔ԍ��B
			bone.no = i;
		}

		fclose(fp);
		return true;
	}
}
