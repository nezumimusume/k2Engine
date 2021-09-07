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
	/// ノードを三角形オブジェクトに変換。
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
	/// パラメータマネージャーからwchar_t型の文字列データを取得する関数。
	/// </summary>
	/// <param name="paramManager">パラメータマネージャ</param>
	/// <param name="stringDst">文字列の格納先</param>
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
	// 構造体
	///////////////////////////////////////////////////
	/// <summary>
	/// 頂点構造体
	/// </summary>
	struct Vertex {
		Point3 pos;					// 座標。
		Point3 normal;				// 法線。
		Point2 uv;					// UV座標。
		std::vector<float> weights;	// スキンウェイト。
		std::vector<int> indices;	// スキンインデックス。
		int texVertNo;				// 
	};
	/// <summary>
	/// マテリアル構造体。
	/// </summary>
	struct Material {
		int id;										// マテリアルID
		std::wstring albedoMapFilePath;				// アルベドマップのファイルパス。
		std::wstring normalMapFilePath;				// 法線マップのファイルパス。
		std::wstring metallicAndSmoothMapFilePath;	// メタリック&スムースマップのファイルパス。
		std::wstring reflectionMap;					// リフレクションマップ(オミット予定)
		std::wstring refractionMap;					// 屈折マップ(オミット予定)
	};
	/// <summary>
	/// インデックスバッファ。
	/// </summary>
	struct IndexBuffer {
		std::vector<int> indices;		// インデックス
	};
	// 各種スマートポインタ
	using VertexPtr = std::shared_ptr<Vertex>;
	using MaterialPtr = std::shared_ptr<Material>;
	using IndexBufferPtr = std::shared_ptr<IndexBuffer>;

	/// <summary>
	/// メッシュパーツ
	/// </summary>
	struct MeshParts {
		std::vector< MaterialPtr > materials;	// マテリアルの配列。
		std::vector< VertexPtr > vertexBuffer;	// 頂点バッファ。
		std::vector< IndexBufferPtr > indexBufferArray; // インデックスバッファの配列(マテリアルの数分だけあります。
	};
	using MeshPartsPtr = std::shared_ptr<MeshParts>;
	/// <summary>
	/// モデルデータクラス。
	/// </summary>
	struct ModelData {
		std::vector< MeshPartsPtr> meshParts;	// メッシュパーツのい解列。
	};
	///////////////////////////////////////////////////
	// メンバ変数。
	///////////////////////////////////////////////////
	std::filesystem::path m_outputFolderPath;	//出力フォルダのパス。
	std::wstring m_ext = L"tkm";	// 拡張子
	std::wstring m_outputFilder;	// 出力フォルダのパス。
	ModelData m_modelData;			// モデルデータ。
	MeshPartsPtr m_meshParts;		// メッシュパーツ。
	std::vector<int> m_newVertNosInFace;	// 面の新しい頂点番号のリスト
	bool m_isFlatSHading;			// フラットシェーディングを行う？
private:
	///////////////////////////////////////////////////
	// privateメンバ関数
	///////////////////////////////////////////////////
	/// <summary>
	/// メッシュパーツを構築(parts of directX material)。
	/// </summary>
	/// <param name="mat"></param>
	void BuildMeshParts_DirectXMaterial(Mtl* mat);
	/// <summary>
	/// メッシュパーツを構築(parts of material)
	/// </summary>
	/// <param name="mat">マテリアル</param>
	void BuildMeshParts_Matrial(Mtl* mat);
	/// <summary>
	/// メッシュパーツを構築(parts of vertexBuffer)。
	/// </summary>
	/// <param name="node"></param>
	void BuildMeshParts_VertexBuffer(INode* node);
	/// <summary>
	/// メッシュパーツを構築(parts of indexBffer)。
	/// </summary>
	void BuildMeshParts_IndexBufer(INode* node);
	/// <summary>
	/// メッシュパーツを構築。
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
		// directXマテリアルではない。
		return;
	}
	// DirectXマテリアル。
	// 新しいマテリアルのインスタンスを作成する。
	MaterialPtr newMaterial = std::make_unique<Material>();
	auto paramManager = dxMat->GetCurrentParameterManager();
	const int numParam = paramManager->GetNumberOfParams();
	for (int paramNo = 0; paramNo < numParam; paramNo++) {

		std::wstring paramName = paramManager->GetParamName(paramNo);
		if (paramName == L"g_albedo") {
			// アルベドテクスチャのファイルパスを取得する。
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->albedoMapFilePath);
		}
		else if (paramName == L"g_normal") {
			// 法線のファイルパスを取得する。
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->normalMapFilePath);
		}
		else if (paramName == L"g_metallicAndSmoothMap") {
			// ファイルパスを取得する。
			GetWStringFromParamManager(paramManager, paramNo, newMaterial->metallicAndSmoothMapFilePath);
		}
	}
	// メッシュパーツに新しいマテリアルを追加。
	m_meshParts->materials.push_back(newMaterial);
}
void tkmExporter::BuildMeshParts_Matrial(Mtl* mat)
{
	if (mat == nullptr) {
		return;
	}
	// マルチマテリアルじゃないか調べる。
	auto multiMat = dynamic_cast<MultiMtl*>(mat);
	if (multiMat) {
		// マルチマテリアル。
		for (int matNo = 0; matNo < multiMat->NumSubMtls(); matNo++) {
			BuildMeshParts_Matrial(multiMat->GetSubMtl(matNo));
		}
	}
	else {
		// マルチマテリアルではない。
		BuildMeshParts_DirectXMaterial(mat);
	}
}
ISkin* FindSkinModifier(Object* obj)
{
	auto pDerivedObj = dynamic_cast<IDerivedObject*>(obj);
	if (pDerivedObj == nullptr) {
		return nullptr;
	}
	// モディファイアスタックがある。
	ISkin* pSkin = nullptr;
	//  スキンモディファイアを探す。
	if (pDerivedObj) {
		// モディファイアが存在している。
		const int numMod = pDerivedObj->NumModifiers();
		for (int modNo = 0; modNo < numMod; modNo++) {
			pSkin = (ISkin*)pDerivedObj->GetModifier(modNo)->GetInterface(I_SKIN);
			if (pSkin != nullptr) {
				// スキンモディファイアを見つけた
				return pSkin;
			}
		}
	}
	return FindSkinModifier(pDerivedObj->GetObjRef());

}
void tkmExporter::BuildMeshParts_VertexBuffer(INode* node)
{
	// maxでは、テクスチャ頂点という概念があり、
	// UV座標のみ別の頂点バッファになっている。
	// k2Engineでは別の頂点バッファになっていると非効率なため、
	// 二つの頂点バッファを結合して一つにする。

	// 3dsMax上での頂点バッファを確保する。
	auto obj = node->GetObjectRef();
	if (obj == nullptr) {
		return;
	}
	
	//  スキンモディファイアを探す。
	ISkin* pSkin = FindSkinModifier(obj);

	auto triObj = GetTriObjectFromNode(node);
	auto& mesh = triObj->GetMesh();
	//　重複頂点を考慮しない、フル頂点の頂点バッファを構築。
	std::vector < VertexPtr > fullVertexBuffer;
	
	// まずはUVなし頂点バッファを構築
	int numVert = mesh.getNumVerts();
	for (int vertNo = 0; vertNo < numVert; vertNo++) {
		// 頂点座標を取得。
		const auto& pos = mesh.getVert(vertNo);
		// 法線を取得。
		auto normal = mesh.getNormal(vertNo);
		// 正規化されてないね。
		normal.Normalize();
		/* todo maxScriptのコードをコピー。意味がよくわからなかったので、要確認。
		
		if skinMod != undefined then(
				--スキンがあるときは、骨で動かされているのでトランスフォームでの動きを除去する？
				--ホンマか？要確認。
				invTrans = inverse c_node.transform
				pos = pos * invTrans
				invRot = c_node.rotation as Matrix3
			--	invRot = inverse invRot
				normal = normal * invRot
			)
		*/
		// スキンウェイト
		float skinWeight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		int skinIndex[] = {-1, -1, -1, -1};
		if (pSkin) {
			// スキンモディファイアがある。
			auto skinContext = pSkin->GetContextInterface(node);
			int numWeight = skinContext->GetNumAssignedBones(vertNo);
			if (numWeight > 4) {
				// スキンウェイトの最大数は4
				numWeight = 4;
			}
			for (int wno = 0; wno < numWeight; wno++) {
				skinWeight[wno] = skinContext->GetBoneWeight(vertNo, wno);
				// 3dsMaxのボーンは頂点に割り当てられているボーンのみ。
				// しかし、k2Engineではすべての骨の情報を必要としているため、
				// 事前に構築しているスケルトン情報からk2Engineで使用する骨番号に変換する。
				int systemBoneNo = skinContext->GetAssignedBone(vertNo, wno);

				 // ここから明日やるぜ。
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
	// 三角形オブジェクトに変換する。
	auto triObject = GetTriObjectFromNode(node);
	if (triObject != nullptr) {
		// 変換出来たらメッシュパーツを構築する。
		m_meshParts = std::make_shared<MeshParts>();

		// マテリアル情報を構築する。
		BuildMeshParts_Matrial(node->GetMtl());

		// 頂点バッファを構築する。
		BuildMeshParts_VertexBuffer(node);

		// インデックスバッファを構築する。
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
