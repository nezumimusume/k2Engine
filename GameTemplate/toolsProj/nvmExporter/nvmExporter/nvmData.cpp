#include "nvmExporter.h"
#include "nvmData.h"
#include <thread>


namespace
{
	//法線が60度を超えたら、壁とみなす。
	const float LIMIT_ANGLE_TO_WALL = 60.0f / 180.0f * 3.1415926f;

}

nvmData::nvmData()
{
	Init();
}

/// <summary>
/// ノードから三角形オブジェクトを取得する。
/// </summary>
/// <param name="node"></param>
/// <returns></returns>
TriObject* nvmData::GetTriObjectFromNode(INode* node)
{
	Object *obj = node->EvalWorldState(0).obj;
	return (TriObject *)(obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0)));
}
/// <summary>
/// 面法線を計算する。
/// </summary>
/// <param name="normal">法線の格納先</param>
/// <param name="face">面</param>
/// <param name="mesh">メッシュ</param>
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

	//面の法線を計算
	normal = Normalize(CrossProd(v0, v1));
}
/// <summary>
/// 引数で渡された面が壁化どうか判定
/// </summary>
/// <param name="normal"></param>
/// <returns></returns>
bool nvmData::IsWall(Point3& normal)
{
	//壁ではない
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
		//メッシュを取得
		Mesh& mesh = triObject->GetMesh();

		int faceNum = mesh.getNumFaces();
		for (int faceNo = 0; faceNo < faceNum; faceNo++) {

			//面を取得
			Face& face = mesh.faces[faceNo];

			//面の法線を計算
			Point3 normal;
			CalcNormal(normal, face, mesh,objectMatrix);

			if (!IsWall(normal)) {
				//各頂点を取得
				std::shared_ptr<SData> data(new SData);
				for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
					DWORD vertexIndex = face.getVert(vertexNo);

					Point3 vertexPosition = mesh.getVert(vertexIndex);
					vertexPosition = objectMatrix.PointTransform(vertexPosition);

					data->position[vertexNo] = vertexPosition;
					data->position[vertexNo][1] = vertexPosition[2];
					data->position[vertexNo][2] = vertexPosition[1] * -1.0f;
					//BSPツリーのリーフを追加していく。
					m_bpsOnVertexPosition.AddLeaf(data->position[vertexNo], data.get());
				}
				//法線
				data->normal = normal;

				//リンク番号のリストは-1で埋める
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
	//BSPツリーを構築。
	m_bpsOnVertexPosition.Build();
}

void nvmData::CreateLinkData(std::vector<SDataPtr>& cellDataArray)
{
	int numFace = m_sDataPtrList.size();
	struct SLinkCellData {
		int numNearVertex = 0;	// 重複頂点がいくつある？
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
					// すでに隣接セルの調査は終わっている。
					return;
				}
				SData* dstData = static_cast<SData*>(leaf->extraData);
				if (srcData.get() == dstData) {
					// 自分の自身なら処理を打ち切る。
					return;
				}
				int faceNo = dstData->faceNo;
				
				Point3 dist = leaf->position - srcData->position[vertexNoSrc];
				int& numNearVertex = linkCellData[faceNo].numNearVertex;
				int* nearVertexNoTbl = linkCellData[faceNo].nearVertexNoTbl;
				if (dist.Length() > 0.01f) {
					// 頂点が離れている。
					return;
				}
				//極めて近い頂点
				linkCellData[faceNo].nearVertexNoTbl[numNearVertex] = vertexNoSrc;
				numNearVertex++;
				if (numNearVertex == 2) {
					//極めて近い頂点が２つ存在するので隣接しているとする
					if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 1)
						|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 1)
						) {
						//頂点番号0-1に隣接している面
						srcData->linkNoList.at(0) = faceNo;
						numLinkCell++;
					}
					else if ((nearVertexNoTbl[0] == 1 && nearVertexNoTbl[1] == 2)
						|| (nearVertexNoTbl[1] == 1 && nearVertexNoTbl[0] == 2)
						) {
						//頂点番号1-2に隣接している面
						srcData->linkNoList.at(1) = faceNo;
						numLinkCell++;
					}
					else if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 2)
						|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 2)
						) {
						//頂点番号0-2に隣接している面
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
	
	// スレッド数。
	const int NUM_THREAD = 4;
	std::vector<SDataPtr> cellDataArrays[NUM_THREAD];
	// 各スレッドに処理させるデータに割り振りを行っていくに割り振っていく。
	if (m_sDataPtrList.size() < 1000) {
		// セルの数が10000に満たない場合はシングルスレッドで行う。
		CreateLinkData(m_sDataPtrList);
	}
	else {
		// 1000以上ならNUM_THREAD数で分担していく。
		int numCellOnThread = m_sDataPtrList.size() / NUM_THREAD;
		int workedCellNo = 0;
		for (int threadNo = 0; threadNo < NUM_THREAD - 1; threadNo++) {
			for (int cellNoInThread = 0; cellNoInThread < numCellOnThread; cellNoInThread++) {
				cellDataArrays[threadNo].push_back(m_sDataPtrList[workedCellNo]);
				workedCellNo++;
			}
		}
		// 末尾のスレッドに残りのセルを全部振り分ける。
		while (workedCellNo < m_sDataPtrList.size()) {
			cellDataArrays[NUM_THREAD-1].push_back(m_sDataPtrList[workedCellNo]);
			workedCellNo++;
		}
		// スレッドを立てる。
		std::unique_ptr<std::thread> thread[NUM_THREAD];
		for (int threadNo = 0; threadNo < NUM_THREAD; threadNo++) {
			thread[threadNo] = std::make_unique<std::thread>([&]()
			{
				CreateLinkData(cellDataArrays[threadNo]);
			});
		}
		// すべてのスレッドが終わるのを待つ。
		for (int threadNo = 0; threadNo < NUM_THREAD; threadNo++) {
			thread[threadNo]->join();
		}
	}
	
	
}