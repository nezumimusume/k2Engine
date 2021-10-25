#include "nvmExporter.h"
#include "nvmExporterCore.h"


namespace {
	/// <summary>
	/// ノードから三角形オブジェクトを取得する。
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node) 
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
	void CalcNormal(Point3& normal, Face& face, Mesh& mesh)
	{
		Point3 vertex[3];


		for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
			DWORD vertexIndex = face.getVert(vertexNo);
			Point3& vertexPosition = mesh.getVert(vertexIndex);
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
	bool IsWall(Point3& normal)
	{
		// todo 未対応。
		//f32 dot = DotProd(normal, Point3(0.0f, 1.0f, 0.0f));

		//if (dot < 0.0f) {
		//	//逆向き
		//	return true;
		//}

		//f32 angle = fabs(acosf(dot));
		//if (angle > Context().GetLimitAngleToWall()) {
		//	//壁
		//	return true;
		//}
		//壁ではない
		return false;
	}
}

void nvmExporterCore::Execute(const std::wstring& filePath)
{
	// ポリゴン情報を収集する。
	auto maxInterface = GetCOREInterface();
	const auto selectNodeCount = maxInterface->GetSelNodeCount();

	for (int nodeNo = 0; nodeNo < selectNodeCount; nodeNo++) {
		auto node = maxInterface->GetSelNode(nodeNo);
		Matrix3 objectMatrix = node->GetObjectTM(0);
		TriObject *triObject = GetTriObjectFromNode(node);
		if (!triObject) {
			continue;
		}
		auto& mesh = triObject->GetMesh();

		int faceNum = mesh.getNumFaces();
		for (int faceNo = 0; faceNo < faceNum; faceNo++) {
			auto& face = mesh.faces[faceNo];
			Point3 normal;
			CalcNormal(normal, face, mesh);

			// 壁判定
			if (!IsWall(normal)) {

			}
		}
		
	}
}