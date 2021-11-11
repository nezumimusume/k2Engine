#include "nvmExporter.h"
#include "nvmData.h"
#include <thread>


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
/// �����œn���ꂽ�ʂ��ǉ��ǂ�������
/// </summary>
/// <param name="normal"></param>
/// <returns></returns>
bool nvmData::IsWall(Point3& normal)
{
	//�ǂł͂Ȃ�
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

	int	num = 0;
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
					//BSP�c���[�̃��[�t��ǉ����Ă����B
					m_bpsOnVertexPosition.AddLeaf(data->position[vertexNo], data.get());
				}
				//�@��
				data->normal = normal;

				//�����N�ԍ��̃��X�g��-1�Ŗ��߂�
				data->linkNoList.push_back(-1);
				data->linkNoList.push_back(-1);
				data->linkNoList.push_back(-1);

				data->faceNo = num;
				num++;

				m_sDataPtrList.push_back(data);

				Vector3 centerPosition;
				centerPosition.x = data->position[0].x + data->position[1].x + data->position[2].x;
				centerPosition.y = data->position[0].y + data->position[1].y + data->position[2].y;
				centerPosition.z = data->position[0].z + data->position[1].z + data->position[2].z;

				
			}
		}
	}
	//BSP�c���[���\�z�B
	m_bpsOnVertexPosition.Build();
}

void nvmData::CreateLinkData(std::vector<SDataPtr>& cellDataArray)
{
	int numFace = m_sDataPtrList.size();
	struct SLinkCellData {
		int numNearVertex = 0;	// �d�����_����������H
		int nearVertexNoTbl[2] = { 0 };
	};
	std::vector<SLinkCellData> linkCellData;
	linkCellData.resize(numFace);

	for (const auto& srcData : cellDataArray)
	{
		memset(&linkCellData.front(), 0, sizeof(SLinkCellData)*linkCellData.size());
		int numLinkCell = 0;
		for (int vertexNoSrc = 0; vertexNoSrc < 3; vertexNoSrc++) {
			m_bpsOnVertexPosition.WalkTree(srcData->position[vertexNoSrc], [&](BSP::SLeaf* leaf) {
				if (numLinkCell == 3) {
					// ���łɗאڃZ���̒����͏I����Ă���B
					return;
				}
				SData* dstData = static_cast<SData*>(leaf->extraData);
				if (srcData.get() == dstData) {
					// �����̎��g�Ȃ珈����ł��؂�B
					return;
				}
				int faceNo = dstData->faceNo;
				
				Point3 dist = leaf->position - srcData->position[vertexNoSrc];
				int& numNearVertex = linkCellData[faceNo].numNearVertex;
				int* nearVertexNoTbl = linkCellData[faceNo].nearVertexNoTbl;
				if (dist.Length() > 0.01f) {
					// ���_������Ă���B
					return;
				}
				//�ɂ߂ċ߂����_
				linkCellData[faceNo].nearVertexNoTbl[numNearVertex] = vertexNoSrc;
				numNearVertex++;
				if (numNearVertex == 2) {
					//�ɂ߂ċ߂����_���Q���݂���̂ŗאڂ��Ă���Ƃ���
					if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 1)
						|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 1)
						) {
						//���_�ԍ�0-1�ɗאڂ��Ă����
						srcData->linkNoList.at(0) = faceNo;
						numLinkCell++;
					}
					else if ((nearVertexNoTbl[0] == 1 && nearVertexNoTbl[1] == 2)
						|| (nearVertexNoTbl[1] == 1 && nearVertexNoTbl[0] == 2)
						) {
						//���_�ԍ�1-2�ɗאڂ��Ă����
						srcData->linkNoList.at(1) = faceNo;
						numLinkCell++;
					}
					else if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 2)
						|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 2)
						) {
						//���_�ԍ�0-2�ɗאڂ��Ă����
						srcData->linkNoList.at(2) = faceNo;
						numLinkCell++;
					}
				}
			});
		}
	}
}

void nvmData::CreateLinkData()
{
	
	// �X���b�h���B
	const int NUM_THREAD = 4;
	std::vector<SDataPtr> cellDataArrays[NUM_THREAD];
	// �e�X���b�h�ɏ���������f�[�^�Ɋ���U����s���Ă����Ɋ���U���Ă����B
	if (m_sDataPtrList.size() < 1000) {
		// �Z���̐���10000�ɖ����Ȃ��ꍇ�̓V���O���X���b�h�ōs���B
		CreateLinkData(m_sDataPtrList);
	}
	else {
		// 1000�ȏ�Ȃ�NUM_THREAD���ŕ��S���Ă����B
		int numCellOnThread = m_sDataPtrList.size() / NUM_THREAD;
		int workedCellNo = 0;
		for (int threadNo = 0; threadNo < NUM_THREAD - 1; threadNo++) {
			for (int cellNoInThread = 0; cellNoInThread < numCellOnThread; cellNoInThread++) {
				cellDataArrays[threadNo].push_back(m_sDataPtrList[workedCellNo]);
				workedCellNo++;
			}
		}
		// �����̃X���b�h�Ɏc��̃Z����S���U�蕪����B
		while (workedCellNo < m_sDataPtrList.size()) {
			cellDataArrays[NUM_THREAD-1].push_back(m_sDataPtrList[workedCellNo]);
			workedCellNo++;
		}
		// �X���b�h�𗧂Ă�B
		std::unique_ptr<std::thread> thread[NUM_THREAD];
		for (int threadNo = 0; threadNo < NUM_THREAD; threadNo++) {
			thread[threadNo] = std::make_unique<std::thread>([&]()
			{
				CreateLinkData(cellDataArrays[threadNo]);
			});
		}
		// ���ׂẴX���b�h���I���̂�҂B
		for (int threadNo = 0; threadNo < NUM_THREAD; threadNo++) {
			thread[threadNo]->join();
		}
	}
	
	
}