#include "k2EngineLowPreCompile.h"
#include "CaslFile.h"
#include <string>

namespace nsK2EngineLow {
	void CaslFile::Load(const char* filePath)
	{
		auto fp = fopen(filePath, "rb");
		//�t�@�C���ǂݍ��߂Ȃ�������B
		if (fp == nullptr) {
			MessageBoxA(nullptr, "casl�t�@�C���̃I�[�v���Ɏ��s���܂����B", "�G���[", MB_OK);
			return;
		}

		//.dds�t�H���_�p�X��ݒ�B�B
		std::string ddsFolderPath = filePath;
		int pos = (int)ddsFolderPath.rfind("/");
		ddsFolderPath = ddsFolderPath.substr(0, static_cast<size_t>(pos) + 1);
		int ddsFolderPathCount = static_cast<int>(ddsFolderPath.length());

		//�摜�̐����擾�B
		int numLevel = ReadInteger(fp);
		for (int i = 0; i < numLevel; i++)
		{

			auto caslData = std::make_unique<CaslData>();
			//std::unique_ptr<Level2DObject> levelObject;
			//���x���t�@�C���ɕۑ����Ă���̂́B
			//�ȉ��S��char�^�ŕۑ����Ă���A����(,)�ŋ�؂肪���Ă���B
			//���O�̒���(int)�A���O(char)�B
			//�t�@�C���p�X�̒���(int)�A�t�@�C���p�X(char)�B
			//���Wx,y(float)�B
			//�摜�̑傫��x,y(int)�B
			//���C���[�D��xx,y(int)�B
			//�X�P�[���{��x,y(float)�B
			//.dds�t�@�C���̖��O�̒���(int)�Adds�t�@�C���̖��O�B

			//�摜�̖��O�̒������擾�B
			int nameCount = ReadInteger(fp);
			//�摜�̖��O���擾�B
			caslData.get()->name = std::make_unique<char[]>(static_cast<size_t>(nameCount) + 1);
			fread(caslData.get()->name.get(), nameCount, 1, fp);
			ReadOnlyOneCharacter(fp);

			//�t�@�C���p�X�̖��O�̒������擾�B
			int fileNameCount = ReadInteger(fp);

			//�t�@�C���p�X���擾�B
			caslData.get()->fileName = std::make_unique<char[]>(static_cast<size_t>(fileNameCount) + 1);
			fread(caslData.get()->fileName.get(), fileNameCount, 1, fp);
			ReadOnlyOneCharacter(fp);

			//���W���擾�B
			caslData.get()->position.x = ReadDecimal(fp);
			caslData.get()->position.y = ReadDecimal(fp);

			//���ƍ������擾�B
			caslData.get()->width = ReadInteger(fp);
			caslData.get()->height = ReadInteger(fp);

			//���C���[�D��x���擾�B
			caslData.get()->numberLayer = ReadInteger(fp);

			//�傫���̔{�����擾�B
			//���݂�1.0f�ɌŒ�B
			ReadDecimal(fp);
			ReadDecimal(fp);
			caslData.get()->scale.x = 1.0f;
			caslData.get()->scale.y = 1.0f;

			//�s�{�b�g���擾�B
			caslData.get()->pivot.x = ReadDecimal(fp);
			caslData.get()->pivot.y = ReadDecimal(fp);

			//dds�t�@�C���p�X�̖��O�̒������擾�B
			int ddsFileNameCount = ReadInteger(fp);
			//dds�t�@�C���p�X���擾�B
			caslData.get()->ddsFileName = std::make_unique<char[]>(static_cast<size_t>(ddsFileNameCount) + 1);
			fread(caslData.get()->ddsFileName.get(), ddsFileNameCount, 1, fp);

			//.dds�t�@�C���p�X��ݒ肷��B
			caslData.get()->ddsFilePath = std::make_unique<char[]>(static_cast<size_t>(ddsFolderPathCount) + static_cast<size_t>(ddsFileNameCount) + 1);
			//�t�H���_�p�X��.dds�t�@�C���̖��O��A������B
			std::string ddsFilePath = ddsFolderPath + caslData.get()->ddsFileName.get();
			std::char_traits<char>::copy(caslData.get()->ddsFilePath.get(), ddsFilePath.c_str(), ddsFilePath.size() + 1);

			//ReadOnlyOneCharacter(fp);
			//���s�R�[�h��ǂݔ�΂��B
			char dummy[256];
			fgets(dummy, 256, fp);

			m_caslDataList.push_back(std::move(caslData));
		}
		//fopen������fclose�ŕ���B
		fclose(fp);
	}

	void CaslFile::ReadOnlyOneCharacter(FILE* file) const
	{
		char a;
		fread(&a, 1, 1, file);
	}

	int CaslFile::ReadInteger(FILE* file) const
	{
		std::string number;
		while (true)
		{
			char c;
			fread(&c, 1, 1, file);
			if (c != ',' && c != '\n')
			{
				number += c;
			}
			else {
				break;
			}
		}
		//string�^��int�^�ɕϊ�����B
		return atoi(number.c_str());
	}

	float CaslFile::ReadDecimal(FILE* file) const
	{
		std::string number;
		while (true)
		{
			char c;
			fread(&c, 1, 1, file);
			if (c != ',' && c != '\n')
			{
				number += c;
			}
			else {
				break;
			}
		}
		//string�^��float�^�ɕϊ�����B
		return static_cast<float>(atof(number.c_str()));
	}
}