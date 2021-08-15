#include "k2EngineLowPreCompile.h"
#include "NullTextureMaps.h"

namespace nsK2EngineLow {
	void NullTextureMaps::Init()
	{
		//�e��e�N�X�`�������[�h�B
		auto TexLoad = [&](
			const char* loadTexFilePath,
			std::unique_ptr<char[]>& outTexData,
			unsigned int& outTexSize
			) {
			FILE* fp = fopen(loadTexFilePath, "rb");
			if (fp == nullptr) {
				//null�e�N�X�`���̃��[�h�Ɏ��s�B
				MessageBoxA(nullptr, "null�e�N�X�`���̃��[�h�Ɏ��s���܂����B", "�G���[", MB_OK);
				std::abort();
			}
			//�e�N�X�`���T�C�Y���v�Z�B
			fseek(fp, 0L, SEEK_END);
			outTexSize = ftell(fp);
			fseek(fp, 0L, SEEK_SET);

			//���������m��
			outTexData = std::make_unique<char[]>(outTexSize);
			fread(outTexData.get(), outTexSize, 1, fp);
			fclose(fp);
		};

		m_albedoMapFilePath = "Assets/modelData/preset/NullAlbedoMap.DDS";
		//�A���x�h�}�b�v�����[�h�B
		TexLoad(
			m_albedoMapFilePath,
			m_albedoMap,
			m_albedoMapSize);

		m_normalMapFilePath = "Assets/modelData/preset/NullNormalMap.DDS";
		//�@���}�b�v�����[�h�B
		TexLoad(
			m_normalMapFilePath,
			m_normalMap,
			m_normalMapSize);

		m_specMapFilePath = "Assets/modelData/preset/specMap_None.DDS";
		//�X�y�L�����}�b�v�}�b�v�����[�h�B
		TexLoad(
			m_specMapFilePath,
			m_specMap,
			m_specMapSize);

		m_zeroValueMapFilePath = "Assets/modelData/preset/ZeroValueMap.DDS";
		//�O�̒l���i�[���Ă���}�b�v�����[�h�B
		TexLoad(
			m_zeroValueMapFilePath,
			m_zeroValueMap,
			m_zeroValueMapSize);
	}
}