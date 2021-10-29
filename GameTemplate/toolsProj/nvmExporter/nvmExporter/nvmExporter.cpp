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

#include "nvmExporter.h"
#include "nvmExporterCore.h"

#define nvmExporter_CLASS_ID Class_ID(0x5ed7efb0, 0x3d531b9d)

class nvmExporter : public SceneExport
{
public:
	// Constructor/Destructor
	nvmExporter();
	virtual ~nvmExporter();

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
};


class nvmExporterClassDesc : public ClassDesc2 
{
public:
	int           IsPublic() override                               { return TRUE; }
	void*         Create(BOOL /*loading = FALSE*/) override         { return new nvmExporter(); }
	const TCHAR*  ClassName() override                              { return GetString(IDS_CLASS_NAME); }
	const TCHAR*  NonLocalizedClassName() override                  { return _T("nvmExporter"); }
	SClass_ID     SuperClassID() override                           { return SCENE_EXPORT_CLASS_ID; }
	Class_ID      ClassID() override                                { return nvmExporter_CLASS_ID; }
	const TCHAR*  Category() override                               { return GetString(IDS_CATEGORY); }

	const TCHAR*  InternalName() override                           { return _T("nvmExporter"); } // Returns fixed parsable name (scripter-visible name)
	HINSTANCE     HInstance() override                              { return hInstance; } // Returns owning module handle


};

ClassDesc2* GetnvmExporterDesc()
{
	static nvmExporterClassDesc nvmExporterDesc;
	return &nvmExporterDesc; 
}




INT_PTR CALLBACK nvmExporterOptionsDlgProc(HWND hWnd, UINT message, WPARAM, LPARAM lParam)
{
	static nvmExporter* imp = nullptr;

	switch (message)
	{
	case WM_INITDIALOG:
		imp = (nvmExporter*)lParam;
		CenterWindow(hWnd, GetParent(hWnd));
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return 1;
	}
	return 0;
}

//--- nvmExporter -------------------------------------------------------
nvmExporter::nvmExporter()
{

}

nvmExporter::~nvmExporter()
{

}

int nvmExporter::ExtCount()
{
	return 1;
}

//拡張子はtkn
const TCHAR* nvmExporter::Ext(int /*i*/)
{
	return _T("tkn");
}

const TCHAR* nvmExporter::LongDesc()
{
	return _T("ナビゲーションメッシュ");
}

const TCHAR* nvmExporter::ShortDesc()
{
	return _T("ナビゲーションメッシュ");
}

const TCHAR* nvmExporter::AuthorName()
{
	return _T("Takayuki Kiyohara");
}

const TCHAR* nvmExporter::CopyrightMessage()
{
	return _T("");
}

const TCHAR* nvmExporter::OtherMessage1()
{
	// TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR* nvmExporter::OtherMessage2()
{
	// TODO: Return other message #2 in any
	return _T("");
}

unsigned int nvmExporter::Version()
{
	return 100;
}

void nvmExporter::ShowAbout(HWND /*hWnd*/)
{
	// Optional
}

BOOL nvmExporter::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
	return TRUE;
}

int nvmExporter::DoExport(const TCHAR* name, ExpInterface* /*ei*/, Interface* /*ip*/, BOOL suppressPrompts, DWORD /*options*/)
{
	nvmExporterCore exporterCore;
	if (exporterCore.Execute(name))
	{
		return TRUE;
	}
	return FALSE;
}
