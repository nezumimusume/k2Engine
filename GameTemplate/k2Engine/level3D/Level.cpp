/*!
 *@brief	レベル。
 */

#include "k2EnginePreCompile.h"
#include "Level.h"
#include "MapChip.h"


Level::~Level()
{
		
}
void Level::CreateMapChip(const LevelObjectData& objData)
{
	//フックされなかったので、マップチップを作成する。
	auto mapChip = std::make_unique<MapChip>(objData);
	m_mapChipPtrs.push_back(std::move(mapChip));
}
void Level::Init(
	const char* filePath,
	std::function<bool(LevelObjectData& objData)> hookFunc
)
{
	//スケルトンをロードする。
	Skeleton skeleton;
	skeleton.Init(filePath);

	//構築構築。
	//0番目はルートオブジェクトなので飛ばす。
	for (auto i = 1; i < skeleton.GetNumBones(); i++) {
		//骨を取得。
		auto bone = skeleton.GetBone(i);
		if (bone->GetParentBoneNo() == 0) {	//親がルートの場合だけマップチップを生成する。
			LevelObjectData objData;
			Vector3 scale;
			bone->CalcWorldTRS(objData.position, objData.rotation, objData.scale);
			//3dsMaxとは軸が違うので、補正を入れる。
			auto t = objData.position.y;
			objData.position.y = objData.position.z;
			objData.position.z = -t;

			t = objData.rotation.y;
			objData.rotation.y = objData.rotation.z;
			objData.rotation.z = -t;

			const wchar_t* wname = bone->GetName();
			//パスの作成
			wchar_t filePath[256];
			swprintf_s(filePath, L"Assets/modelData/%s.tkm", wname);
			//マルチバイトに変換 wchar→ char
			//ベースの文字列。
			size_t origsize = wcslen(filePath) + 1;
			//変換した文字列
			size_t convertedChars = 0;
			//マルチバイト格納用
			char strCon[] = "";
			//サイズ
			size_t strConSize = (strlen(strCon) + 1) * 2;
			//変換後のサイズ。
			const size_t newsize = origsize * 2;
			//連結後のサイズ
			objData.name = new char[newsize + strConSize];
			//マルチバイトに変換する。入りきらないものは切りすて
			wcstombs_s(&convertedChars, objData.name, newsize, filePath, _TRUNCATE);
			//文字列の連結
			_mbscat_s((unsigned char*)objData.name, newsize + strConSize, (unsigned char*)strCon);


			std::swap(objData.scale.y, objData.scale.z);

			auto isHook = false;
			if (hookFunc != nullptr) {
				//hook関数が指定されているのでhook関数を呼び出す。
				isHook = hookFunc(objData);
			}
			if (isHook == false) {
				//マップチップを作成する。
				CreateMapChip(objData);
			}
		}
	}
}
