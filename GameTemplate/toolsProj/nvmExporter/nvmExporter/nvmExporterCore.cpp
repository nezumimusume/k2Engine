#include "nvmExporter.h"
#include "nvmExporterCore.h"

#include "nvmData.h"


bool nvmExporterCore::Execute(const std::wstring& filePath)
{
	// ファイルオープン
	FILE* fp = _wfopen(filePath.c_str(), L"wb");

	nvmData data;

	int faceNum = data.GetFaceNum();
	// ヘッダーの出力
	// 面(ポリゴン)数の出力。
	fwrite(&faceNum, sizeof(int), 1, fp);
	for (int faceNo = 0; faceNo < faceNum; faceNo++) {
		for (int vertexNo = 0; vertexNo < 3; vertexNo++) {
			Point3 position = data.GetVertexPosition(faceNo, vertexNo);
			//各ポリゴンの頂点の座標を出力
			fwrite(&position[0], sizeof(float), 1, fp);
			fwrite(&position[1], sizeof(float), 1, fp);
			fwrite(&position[2], sizeof(float), 1, fp);
		}

		Point3 normal = data.GetNormal(faceNo);

		// 法線を出力
		fwrite(&normal[0], sizeof(float), 1, fp);
		fwrite(&normal[1], sizeof(float), 1, fp);
		fwrite(&normal[2], sizeof(float), 1, fp);


		
		for (int linkNoListNo = 0; linkNoListNo < nvmData::LINK_NO_LIST_SIZE; linkNoListNo++) {
			int linkNo = data.GetLinkNo(faceNo, linkNoListNo);
			// リンクデータを出力
			fwrite(&linkNo, sizeof(linkNo), 1, fp);
		}
	}

	fclose(fp);
	return true;
	
}