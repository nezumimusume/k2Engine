#include "nvmExporter.h"
#include "nvmExporterCore.h"

#include "nvmData.h"


bool nvmExporterCore::Execute(const std::wstring& filePath)
{
	// �t�@�C���I�[�v��
	FILE* fp = _wfopen(filePath.c_str(), L"wb");

	nvmData data;

	int faceNum = data.GetFaceNum();
	// �w�b�_�[�̏o��
	// ��(�|���S��)���̏o�́B
	fwrite(&faceNum, sizeof(int), 1, fp);
	for (int faceNo = 0; faceNo < faceNum; faceNo++) {
		for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
			Point3 position = data.GetVertexPosition(faceNo, vertexNo);
			//�e�|���S���̒��_�̍��W���o��
			fwrite(&position[0], sizeof(float), 1, fp);
			fwrite(&position[1], sizeof(float), 1, fp);
			fwrite(&position[2], sizeof(float), 1, fp);
		}

		Point3 normal = data.GetNormal(faceNo);

		// �@�����o��
		fwrite(&normal[0], sizeof(float), 1, fp);
		fwrite(&normal[1], sizeof(float), 1, fp);
		fwrite(&normal[2], sizeof(float), 1, fp);


		
		for (int linkNoListNo = 0; linkNoListNo < nvmData::LINK_NO_LIST_SIZE; linkNoListNo++) {
			int linkNo = data.GetLinkNo(faceNo, linkNoListNo);
			// �����N�f�[�^���o��
			fwrite(&linkNo, sizeof(linkNo), 1, fp);
		}
	}

	fclose(fp);
	return true;
	
}