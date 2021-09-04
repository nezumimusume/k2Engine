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

#define tkmExporter_CLASS_ID Class_ID(0x85d8d367, 0x973ecfa3)

namespace {
	/// <summary>
	/// ノードを三角形オブジェクトに変換。
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	TriObject* GetTriObjectFromNode(INode* node) 
	{
		Object *obj = node->EvalWorldState(0).obj;
		return (TriObject *)(obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0)));
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
	/// マテリアル情報を構築する。
	/// </summary>
	void BuildMatrialData(INode* node);
	/// <summary>
	/// メッシュパーツを構築
	/// </summary>
	/// <param name="node">構築するノード</param>
	void BuildMeshParts_Matrial(INode* node);
	/// <summary>
	/// 頂点バッファを構築。
	/// </summary>
	/// <param name="node"></param>
	void BuildMeshParts_VertexBuffer(INode* node);
	/// <summary>
	/// インデックスバッファを構築。
	/// </summary>
	void BuildMeshParts_IndexBufer(INode* node);
	
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
#pragma message(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return m_ext.c_str();
}

const TCHAR* tkmExporter::LongDesc()
{
#pragma message(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\")"))
	return _T("3DModel exporter that format for k2Engine.");
}

const TCHAR* tkmExporter::ShortDesc()
{
#pragma message(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("tkm");
}

const TCHAR* tkmExporter::AuthorName()
{
#pragma message(TODO("Return ASCII Author name"))
	return _T("Takayuki Kiyohara");
}

const TCHAR* tkmExporter::CopyrightMessage()
{
#pragma message(TODO("Return ASCII Copyright message"))
	return _T("");
}

const TCHAR* tkmExporter::OtherMessage1()
{
	// TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR* tkmExporter::OtherMessage2()
{
	// TODO: Return other message #2 in any
	return _T("");
}

unsigned int tkmExporter::Version()
{
#pragma message(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void tkmExporter::ShowAbout(HWND /*hWnd*/)
{
	// Optional
}

BOOL tkmExporter::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
#pragma message(TODO("Decide which options to support.  Simply return true for each option supported by each Extension the exporter supports."))
	return TRUE;
}
void tkmExporter::BuildMeshParts_Matrial(INode* node)
{

}
void tkmExporter::BuildMeshParts_VertexBuffer(INode* node)
{

}
void tkmExporter::BuildMeshParts_IndexBufer(INode* node)
{

}
void tkmExporter::BuildMeshParts(INode* node)
{
	// マテリアル情報を構築する。
	BuildMeshParts_Matrial(node);

	// 頂点バッファを構築する。
	BuildMeshParts_VertexBuffer(node);

	// インデックスバッファを構築する。
	BuildMeshParts_IndexBufer(node);

	auto numChild = node->NumChildren();
	for (int childNo = 0; childNo < numChild; childNo++) {
		auto childNode = node->GetChildNode(childNo);
		auto triObject = GetTriObjectFromNode(childNode);
		if (triObject != nullptr) {
			BuildMeshParts(node);
		}
	}
	
}
int tkmExporter::DoExport(const TCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options)
{
	
	auto rootNode = ip->GetRootNode();
	auto numChild = rootNode->NumChildren();
	for (int childNo = 0; childNo < numChild; childNo++) {
		auto childNode = rootNode->GetChildNode(childNo);
		auto triObject = GetTriObjectFromNode(childNode);
		if (triObject != nullptr) {
			// 三角形オブジェクトに変換できた。
			// メッシュパーツを構築
			BuildMeshParts(childNode);
			wchar_t hoge[256];
			wsprintf(hoge, L"%ls\n", childNode->NodeName());
			OutputDebugString(hoge);
		}
	}
	return FALSE;
}
