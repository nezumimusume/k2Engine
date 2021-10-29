#include "nvmExporter.h"
#include "nvmData.h"


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
		Point3& vertexPosition = mesh.getVert(vertexIndex);
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
	// todo 未対応。
	float dot = DotProd(normal, Point3(0.0f, 1.0f, 0.0f));

	if (dot < 0.0f) {
		//逆向き
		return true;
	}

	float angle = fabs(acosf(dot));
	if (angle > LIMIT_ANGLE_TO_WALL) {
		//壁
		return true;
	}
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

				}
				//法線
				data->normal = normal;

				//リンク番号のリストは-1で埋める
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
				//自分以外のデータで極めて近い頂点が２つ存在したら隣接している面とする
				int nearVertexNum = 0;
				int nearVertexNoTbl[2] = { 0 };
				for (int vertexNoSrc = 0; vertexNoSrc < 3 && nearVertexNum < 2; vertexNoSrc++) {
					Point3& srcPoint = srcData->position[vertexNoSrc];
					for (int vertexNoDst = 0; vertexNoDst < 3; vertexNoDst++) {
						Point3& dstPoint = dstData->position[vertexNoDst];
						Point3 dist;
						dist = srcPoint - dstPoint;
						if (dist.Length() < 0.1f) {
							//極めて近い頂点
							nearVertexNoTbl[nearVertexNum] = vertexNoSrc;
							nearVertexNum++;
							if (nearVertexNum == 2) {
								//極めて近い頂点が２つ存在するので隣接しているとする
								if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 1)
									|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 1)
									) {
									//頂点番号0-1に隣接している面
									srcData->linkNoList.at(0) = faceNo;
								}
								else if ((nearVertexNoTbl[0] == 1 && nearVertexNoTbl[1] == 2)
									|| (nearVertexNoTbl[1] == 1 && nearVertexNoTbl[0] == 2)
									) {
									//頂点番号1-2に隣接している面
									srcData->linkNoList.at(1) = faceNo;
								}
								else if ((nearVertexNoTbl[0] == 0 && nearVertexNoTbl[1] == 2)
									|| (nearVertexNoTbl[1] == 0 && nearVertexNoTbl[0] == 2)
									) {
									//頂点番号0-2に隣接している面
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