//**************************************************************************/
// Copyright (c) 1998-2020 Autodesk, Inc.
// All rights reserved.
// 
// Use of this software is subject to the terms of the Autodesk license 
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Plugin Wizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "tkmExporter.h"
#include <vector>
#include <filesystem>
#include "AssetManagement/AssetUser.h"
#include "IDxMaterial.h"
#include <RTMax.h>
#include <functional>
#include <modstack.h>
#include <iskin.h>
#include <triobj.h>

#define tkmExporter_CLASS_ID Class_ID(0x85d8d367, 0x973ecfa3)

namespace {
	/// <summary>
	/// �m�[�h���O�p�`�I�u�W�F�N�g�ɕϊ��B
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node) 
	{
		auto name = node->GetName();
		Object *obj = node->EvalWorldState(0).obj;
		if (obj) {
			return (TriObject *)(obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0)));
		}
		return nullptr;
	}
	/// <summary>
	/// �p�����[�^�}�l�[�W���[����wchar_t�^�̕�����f�[�^���擾����֐��B
	/// </summary>
	/// <param name="paramManager">�p�����[�^�}�l�[�W��</param>
	/// <param name="stringDst">������̊i�[��</param>
	void GetWStringFromParamManager(IParameterManager* paramManager, int paramNo, std::wstring& stringDst)
	{
		std::wstring paramName = paramManager->GetParamName(paramNo);
		auto size = paramManager->GetParamSize(paramNo);
		stringDst.resize(size + 1);
		paramManager->GetParamData(&stringDst.front(), paramName.c_str());
	}
}
class tkmExporter : public SceneExport
{
private:
	///////////////////////////////////////////////////
	// �\����
	///////////////////////////////////////////////////
	/// <summary>
	/// ���_�\����
	/// </summary>
	struct Vertex {
		Point3 pos;					// ���W�B
		Point3 normal;				// �@���B
		Point2 uv;					// UV���W�B
		std::vector<float> weights;	// �X�L���E�F�C�g�B
		std::vector<int> indices;	// �X�L���C���f�b�N�X�B
		int texVertNo;				// 
	};
	/// <summary>
	/// �}�e���A���\���́B
	/// </summary>
	struct Material {
		int id;										// �}�e���A��ID
		std::wstring albedoMapFilePath;				// �A���x�h�}�b�v�̃t�@�C���p�X�B
		std::wstring normalMapFilePath;				// �@���}�b�v�̃t�@�C���p�X�B
		std::wstring metallicAndSmoothMapFilePath;	// ���^���b�N&�X���[�X�}�b�v�̃t�@�C���p�X�B
		std::wstring reflectionMap;					// ���t���N�V�����}�b�v(�I�~�b�g�\��)
		std::wstring refractionMap;					// ���܃}�b�v(�I�~�b�g�\��)
	};
	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�B
	/// </summary>
	struct IndexBuffer {
		std::vector<int> indices;		// �C���f�b�N�X
	};
	// �e��X�}�[�g�|�C���^
	using VertexPtr = std::shared_ptr<Vertex>;
	using MaterialPtr = std::shared_ptr<Material>;
	using IndexBufferPtr = std::shared_ptr<IndexBuffer>;

	/// <summary>
	/// ���b�V���p�[�c
	/// </summary>
	struct MeshParts {
		std::vector< MaterialPtr > materials;	// �}�e���A���̔z��B
		std::vector< VertexPtr > vertexBuffer;	// ���_�o�b�t�@�B
		std::vector< IndexBufferPtr > indexBufferArray; // �C���f�b�N�X�o�b�t�@�̔z��(�}�e���A���̐�����������܂��B
	};
	using MeshPartsPtr = std::shared_ptr<MeshParts>;
	/// <summary>
	/// ���f���f�[�^�N���X�B
	/// </summary>
	struct ModelData {
		std::vector< MeshPartsPtr> meshParts;	// ���b�V���p�[�c�̂����B
	};
	///////////////////////////////////////////////////
	// �����o�ϐ��B
	///////////////////////////////////////////////////
	std::filesystem::path m_outputFolderPath;	//�o�̓t�H���_�̃p�X�B
	std::wstring m_ext = L"tkm";	// �g���q
	std::wstring m_outputFilder;	// �o�̓t�H���_�̃p�X�B
	ModelData m_modelData;			// ���f���f�[�^�B
	MeshPartsPtr m_meshParts;		// ���b�V���p�[�c�B
	std::vector<int> m_newVertNosInFace;	// �ʂ̐V�������_�ԍ��̃��X�g
	bool m_isFlatSHading;			// �t���b�g�V�F�[�f�B���O���s���H
private:
	///////////////////////////////////////////////////
	// private�����o�֐�
	///////////////////////////////////////////////////
	/// <summary>
	/// ���b�V���p�[�c���\�z(parts of directX material)�B
	/// </summary>
	/// <param name="mat"></param>
	void BuildMeshParts_DirectXMaterial(Mtl* mat);
	/// <summary>
	/// ���b�V���p�[�c���\�z(parts of material)
	/// </summary>
	/// <param name="mat">�}�e���A��</param>
	void BuildMeshParts_Matrial(Mtl* mat);
	/// <summary>
	/// ���b�V���p�[�c���\�z(parts of vertexBuffer)�B
	/// </summary>
	/// <param name="node"></param>
	void BuildMeshParts_VertexBuffer(INode* node);
	/// <summary>
	/// ���b�V���p�[�c���\�z(parts of indexBffer)�B
	/// </summary>
	void BuildMeshParts_IndexBufer(INode* node);
	/// <summary>
	/// ���b�V���p�[�c���\�z�B
	/// </summary>
	/// <param name="node"></param>
	void BuildMeshParts(INode* node);
	
public:
	// Constructor/Destructor
	tkmExporter();
	virtual ~tkmExporter();

	int ExtCount(); // Number of extensions supported
	const TCHAR* Ext(int n); // Extension #n (i.e. "3DS")
	const TCHAR* LongDesc(); // Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR* ShortDesc(); // Short ASCII description (i.e. "3D Studio")
	const TCHAR* AuthorName(); // ASCII Author name
	const TCHAR* CopyrightMessage(); // ASCII Copyright message
	const TCHAR* OtherMessage1(); // Other message #1
	const TCHAR* OtherMessage2(); // Other message #2
	unsigned int Version(); // Version number * 100 (i.e. v3.01 = 301)
	void ShowAbout(HWND hWnd); // Show DLL's "About..." box

	BOOL SupportsOptions(int ext, DWORD options);
	int DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0);
private:
	
};


class tkmExporterClassDesc : public ClassDesc2 
{
public:
	int           IsPublic() override                               { return TRUE; }
	void*         Create(BOOL /*loading = FALSE*/) override         { return new tkmExporter(); }
	const TCHAR*  ClassName() override                              { return GetString(IDS_CLASS_NAME); }
	const TCHAR*  NonLocalizedClassName() override                  { return _T("tkmExporter"); }
	SClass_ID     SuperClassID() override                           { return SCENE_EXPORT_CLASS_ID; }
	Class_ID      ClassID() override                                { return tkmExporter_CLASS_ID; }
	const TCHAR*  Category() override                               { return GetString(IDS_CATEGORY); }

	const TCHAR*  InternalName() override                           { return _T("tkmExporter"); } // Returns fixed parsable name (scripter-visible name)
	HINSTANCE     HInstance() override                              { return hInstance; } // Returns owning module handle


};

ClassDesc2* GettkmExporterDesc()
{
	static tkmExporterClassDesc tkmExporterDesc;
	return &tkmExporterDesc; 
}




INT_PTR CALLBACK tkmExporterOptionsDlgProc(HWND hWnd, UINT message, WPARAM, LPARAM lParam)
{
	static tkmExporter* imp = nullptr;

	switch (message)
	{
	case WM_INITDIALOG:
		imp = (tkmExporter*)lParam;
		CenterWindow(hWnd, GetParent(hWnd));
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return 1;
	}
	return 0;
}

//--- tkmExporter -------------------------------------------------------
tkmExporter::tkmExporter()
{

}

tkmExporter::~tkmExporter()
{

}


int tkmExporter::ExtCount()
{
	return 1;
}

const TCHAR* tkmExporter::Ext(int /*i*/)
{
	return m_ext.c_str();
}

const TCHAR* tkmExporter::LongDesc()
{
	return _T("3DModel exporter that format for k2Engine.");
}

const TCHAR* tkmExporter::ShortDesc()
{
	return _T("tkm");
}

const TCHAR* tkmExporter::AuthorName()
{
	return _T("Takayuki Kiyohara");
}

const TCHAR* tkmExporter::CopyrightMessage()
{
	return _T("");
}

const TCHAR* tkmExporter::OtherMessage1()
{
	return _T("");
}

const TCHAR* tkmExporter::OtherMessage2()
{
	return _T("");
}

unsigned int tkmExporter::Version()
{
	return 100;
}

void tkmExporter::ShowAbout(HWND /*hWnd*/)
{
	// Optional
}

BOOL tkmExporter::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
	return TRUE;
}
void tkmExporter::BuildMeshParts_DirectXMaterial(Mtl* mat)
{
	auto dxMat = dynamic_cast<IDxMaterial3*>(mat->GetInterface(IDXMATERIAL3_INTERFACE));
	if (dxMat == nullptr) {
		// directX�}�e���A���ł͂Ȃ��B
		return;
	}
	// DirectX�}�e���A���B
	// �V�����}�e���A���̃C���X�^���X���쐬����B
	MaterialPtr newMaterial = std::make_unique<Material>();
	auto paramManager = dxMat->GetCurrentParameterManager();
	const int numParam = paramManager->GetNumberOfParams();
	for (int paramNo = 0; paramNo < numParam; paramNo++) {

		std::wstring paramName = paramManager->GetParamName(paramNo);
		if (paramName == L"g_albedo") {
			// �A���x�h�e�N�X�`���̃t�@�C���p�X���擾����B
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->albedoMapFilePath);
		}
		else if (paramName == L"g_normal") {
			// �@���̃t�@�C���p�X���擾����B
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->normalMapFilePath);
		}
		else if (paramName == L"g_metallicAndSmoothMap") {
			// �t�@�C���p�X���擾����B
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->metallicAndSmoothMapFilePath);
		}
	}
	// ���b�V���p�[�c�ɐV�����}�e���A����ǉ��B
	m_meshParts->materials.push_back(newMaterial);
}
void tkmExporter::BuildMeshParts_Matrial(Mtl* mat)
{
	if (mat == nullptr) {
		return;
	}
	// �}���`�}�e���A������Ȃ������ׂ�B
	auto multiMat = dynamic_cast<MultiMtl*>(mat);
	if (multiMat) {
		// �}���`�}�e���A���B
		for (int matNo = 0; matNo < multiMat->NumSubMtls(); matNo++) {
			BuildMeshParts_Matrial(multiMat->GetSubMtl(matNo));
		}
	}
	else {
		// �}���`�}�e���A���ł͂Ȃ��B
		BuildMeshParts_DirectXMaterial(mat);
	}
}
ISkin* FindSkinModifier(Object* obj)
{
	auto pDerivedObj = dynamic_cast<IDerivedObject*>(obj);
	if (pDerivedObj == nullptr) {
		return nullptr;
	}
	// ���f�B�t�@�C�A�X�^�b�N������B
	ISkin* pSkin = nullptr;
	//  �X�L�����f�B�t�@�C�A��T���B
	if (pDerivedObj) {
		// ���f�B�t�@�C�A�����݂��Ă���B
		const int numMod = pDerivedObj->NumModifiers();
		for (int modNo = 0; modNo < numMod; modNo++) {
			pSkin = (ISkin*)pDerivedObj->GetModifier(modNo)->GetInterface(I_SKIN);
			if (pSkin != nullptr) {
				// �X�L�����f�B�t�@�C�A��������
				return pSkin;
			}
		}
	}
	return FindSkinModifier(pDerivedObj->GetObjRef());

}
void tkmExporter::BuildMeshParts_VertexBuffer(INode* node)
{
	// max�ł́A�e�N�X�`�����_�Ƃ����T�O������A
	// UV���W�̂ݕʂ̒��_�o�b�t�@�ɂȂ��Ă���B
	// k2Engine�ł͕ʂ̒��_�o�b�t�@�ɂȂ��Ă���Ɣ�����Ȃ��߁A
	// ��̒��_�o�b�t�@���������Ĉ�ɂ���B

	// 3dsMax��ł̒��_�o�b�t�@���m�ۂ���B
	auto obj = node->GetObjectRef();
	if (obj == nullptr) {
		return;
	}
	
	//  �X�L�����f�B�t�@�C�A��T���B
	ISkin* pSkin = FindSkinModifier(obj);

	auto triObj = GetTriObjectFromNode(node);
	auto& mesh = triObj->GetMesh();
	//�@�d�����_���l�����Ȃ��A�t�����_�̒��_�o�b�t�@���\�z�B
	std::vector < VertexPtr > fullVertexBuffer;
	
	// �܂���UV�Ȃ����_�o�b�t�@���\�z
	int numVert = mesh.getNumVerts();
	for (int vertNo = 0; vertNo < numVert; vertNo++) {
		// ���_���W���擾�B
		const auto& pos = mesh.getVert(vertNo);
		// �@�����擾�B
		auto normal = mesh.getNormal(vertNo);
		// ���K������ĂȂ��ˁB
		normal.Normalize();
		/* todo maxScript�̃R�[�h���R�s�[�B�Ӗ����悭�킩��Ȃ������̂ŁA�v�m�F�B
		
		if skinMod != undefined then(
				--�X�L��������Ƃ��́A���œ�������Ă���̂Ńg�����X�t�H�[���ł̓�������������H
				--�z���}���H�v�m�F�B
				invTrans = inverse c_node.transform
				pos = pos * invTrans
				invRot = c_node.rotation as Matrix3
			--	invRot = inverse invRot
				normal = normal * invRot
			)
		*/
		// �X�L���E�F�C�g
		float skinWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		int skinIndex[] = {-1, -1, -1, -1};
		if (pSkin) {
			// �X�L�����f�B�t�@�C�A������B
			auto skinContext = pSkin->GetContextInterface(node);
			int numWeight = skinContext->GetNumAssignedBones(vertNo);
			if (numWeight > 4) {
				// �X�L���E�F�C�g�̍ő吔��4
				numWeight = 4;
			}
			for (int wno = 0; wno < numWeight; wno++) {
				skinWeight[wno] = skinContext->GetBoneWeight(vertNo, wno);
				// 3dsMax�̃{�[���͒��_�Ɋ��蓖�Ă��Ă���{�[���̂݁B
				// �������Ak2Engine�ł͂��ׂĂ̍��̏���K�v�Ƃ��Ă��邽�߁A
				// ���O�ɍ\�z���Ă���X�P���g����񂩂�k2Engine�Ŏg�p���鍜�ԍ��ɕϊ�����B
				int systemBoneNo = skinContext->GetAssignedBone(vertNo, wno);

				 // �������疾����邺�B
				auto boneName = pSkin->GetBoneName(systemBoneNo);
				printf( "hoge\n" );
				
			}
		}
	}
	
	
	

}
void tkmExporter::BuildMeshParts_IndexBufer(INode* node)
{

}
void tkmExporter::BuildMeshParts(INode* node)
{
	// �O�p�`�I�u�W�F�N�g�ɕϊ�����B
	auto triObject = GetTriObjectFromNode(node);
	if (triObject != nullptr) {
		// �ϊ��o�����烁�b�V���p�[�c���\�z����B
		m_meshParts = std::make_shared<MeshParts>();

		// �}�e���A�������\�z����B
		BuildMeshParts_Matrial(node->GetMtl());

		// ���_�o�b�t�@���\�z����B
		BuildMeshParts_VertexBuffer(node);

		// �C���f�b�N�X�o�b�t�@���\�z����B
		BuildMeshParts_IndexBufer(node);
	}
	auto numChild = node->NumChildren();
	for (int childNo = 0; childNo < numChild; childNo++) {
		auto childNode = node->GetChildNode(childNo);
		BuildMeshParts(childNode);
	}
	
}
int tkmExporter::DoExport(const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options)
{
	std::filesystem::path outputFilePath;
	outputFilePath = name;
	m_outputFolderPath = outputFilePath.remove_filename();

	auto rootNode = ip->GetRootNode();
	BuildMeshParts(rootNode);
	
	return FALSE;
}
