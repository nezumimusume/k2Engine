#include "nvmExporter.h"
#include "nvmData.h"


namespace
{
	//�@����60�x�𒴂�����A�ǂƂ݂Ȃ��B
	const float LIMIT_ANGLE_TO_WALL = 60.0f / 180.0f * 3.1415926f;

}

nvmData::nvmData()
{
	Init();
}

/// <summary>
/// �m�[�h����O�p�`�I�u�W�F�N�g���擾����B
/// </summary>
/// <param name="node"></param>
/// <returns></returns>
TriObject* nvmData::GetTriObjectFromNode(INode* node)
{
	Object *obj = node->EvalWorldState(0).obj;
	return (TriObject *)(obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0)));
}
/// <summary>
/// �ʖ@�����v�Z����B
/// </summary>
/// <param name="normal">�@���̊i�[��</param>
/// <param name="face">��</param>
/// <param name="mesh">���b�V��</param>
void nvmData::CalcNormal(Point3& normal, Face& face, Mesh& mesh, Matrix3& matrix)
{
	Point3 vertex[3];


	for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
		DWORD vertexIndex = face.getVert(vertexNo);
		Point3 vertexPosition = mesh.getVert(vertexIndex);
		vertexPosition = matrix.PointTransform(vertexPosition);
		vertex[vertexNo] = vertexPosition;
		vertex[vertexNo][1] = vertexPosition[2];
		vertex[vertexNo][2] = vertexPosition[1] * -1.0f;
	}

	Point3 v0, v1;
	v0 = vertex[1] - vertex[0];
	v1 = vertex[2] - vertex[1];

	if (v0.LengthSquared() < 0.00001f) {
		normal = Point3(0.0f, 0.0f, 0.0f);
		return;
	}
	if (v1.LengthSquared() < 0.00001f) {
		normal = Point3(0.0f, 0.0f, 0.0f);
		return;
	}
	v0 = Normalize(v0);
	v1 = Normalize(v1);

	//�ʂ̖@�����v�Z
	normal = Normalize(CrossProd(v0, v1));
}
/// <summary>
/// �����œn���ꂽ�ʂ��ǂ��ǂ�������
/// </summary>
/// <param name="normal"></param>
/// <returns></returns>
bool nvmData::IsWall(Point3& normal)
{
	return false;
}

void nvmData::Init()
{
	CreatePositionAndNormalData();
	CreateLinkData();
}

void nvmData::CreatePositionAndNormalData()
{
	Interface *max = GetCOREInterface();
	int selNodeCnt = max->GetSelNodeCount();

	for (int nodeNo = 0; nodeNo < selNodeCnt; nodeNo++) {
		INode* node = max->GetSelNode(nodeNo);
		Matrix3 objectMatrix = node->GetObjectTM(0);
		TriObject *triObject = GetTriObjectFromNode(node);
		if (!triObject) {
			continue;
		}
		//���b�V�����擾
		Mesh& mesh = triObject->GetMesh();

		int faceNum = mesh.getNumFaces();
		for (int faceNo = 0; faceNo < faceNum; faceNo++) {

			//�ʂ��擾
			Face& face = mesh.faces[faceNo];

			//�ʂ̖@�����v�Z
			Point3 normal;
			CalcNormal(normal, face, mesh,objectMatrix);

			if (!IsWall(normal)) {
				//�e���_���擾
				std::shared_ptr<SData> data(new SData);
				for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
					DWORD vertexIndex = face.getVert(vertexNo);

					Point3 vertexPosition = mesh.getVert(vertexIndex);
					vertexPosition = objectMatrix.PointTransform(vertexPosition);

					data->position[vertexNo] = vertexPosition;
					data->position[vertexNo][1] = vertexPosition[2];
					data->position[vertexNo][2] = vertexPosition[1] * -1.0f;

				}
				//�@��
				data->normal = normal;

				//�����N�ԍ��̃��X�g��-1�Ŗ��߂�
				data->linkNoList.push_back(-1);
				data->linkNoList.push_back(-1);
				data->linkNoList.push_back(-1);

				m_sDataPtrList.push_back(data);
			}
		}
	}
}


void nvmData::CreateLinkData()
{
	for (
		auto itSrc = m_sDataPtrList.begin();
		itSrc != m_sDataPtrList.end();
		itSrc++
		) {
		const auto& srcData = *itSrc;
		int faceNo = 0;
		for (
			auto itDst = m_sDataPtrList.begin();
			itDst != m_sDataPtrList.end();
			itDst++, faceNo++
			) {
			const auto& dstData = *itDst;
			if (srcData != dstData) {
				//�����ȊO�̃f�[�^�ŋɂ߂ċ߂����_���Q���݂�����אڂ��Ă���ʂƂ���
				int nearVertexNum = 0;
				int nearVertexNoTbl[2] = { 0 };
				for (int vertexNoSrc = 0; vertexNoSrc < 3 && nearVertexNum < 2; vertexNoSrc++) {
					Point3& srcPoint = srcData->position[vertexNoSrc];
					for (int vertexNoDst = 0; vertexNoDst < 3; vertexNoDst++) {
						Point3& dstPoint = dstData->position[vertexNoDst];
						Point3 dist;
						dist = srcPoint - dstPoint;
						if (dist.Length() < 0.1f) {
							//�ɂ߂ċ߂����_
							nearVertexNoTbl[nearVertexNum] = vertexNoSrc;
							nearVertexNum++;
							if (nearVertexNum == 2) {
								//�ɂ߂ċ߂����_���Q���݂���̂ŗאڂ��Ă���Ƃ���
								if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 1)
									|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 1)
									) {
									//���_�ԍ�0-1�ɗאڂ��Ă����
									srcData->linkNoList.at(0) = faceNo;
								}
								else if ((nearVertexNoTbl[0] == 1 && nearVertexNoTbl[1] == 2)
									|| (nearVertexNoTbl[1] == 1 && nearVertexNoTbl[0] == 2)
									) {
									//���_�ԍ�1-2�ɗאڂ��Ă����
									srcData->linkNoList.at(1) = faceNo;
								}
								else if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 2)
									|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 2)
									) {
									//���_�ԍ�0-2�ɗאڂ��Ă����
									srcData->linkNoList.at(2) = faceNo;
								}
								break;
							}
						}
					}
				}

			}
		}
	}
}