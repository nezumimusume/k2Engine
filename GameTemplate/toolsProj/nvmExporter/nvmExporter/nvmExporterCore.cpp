#include "nvmExporter.h"
#include "nvmExporterCore.h"


namespace {
	/// <summary>
	/// �m�[�h����O�p�`�I�u�W�F�N�g���擾����B
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node) 
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

		//�ʂ̖@�����v�Z
		normal = Normalize(CrossProd(v0, v1));
	}
	/// <summary>
	/// �����œn���ꂽ�ʂ��ǉ��ǂ�������
	/// </summary>
	/// <param name="normal"></param>
	/// <returns></returns>
	bool IsWall(Point3& normal)
	{
		// todo ���Ή��B
		//f32 dot = DotProd(normal, Point3(0.0f, 1.0f, 0.0f));

		//if (dot < 0.0f) {
		//	//�t����
		//	return true;
		//}

		//f32 angle = fabs(acosf(dot));
		//if (angle > Context().GetLimitAngleToWall()) {
		//	//��
		//	return true;
		//}
		//�ǂł͂Ȃ�
		return false;
	}
}

void nvmExporterCore::Execute(const std::wstring& filePath)
{
	// �|���S���������W����B
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

			// �ǔ���
			if (!IsWall(normal)) {

			}
		}
		
	}
}