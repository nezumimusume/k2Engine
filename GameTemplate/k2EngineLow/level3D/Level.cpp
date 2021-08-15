/*!
 *@brief	レベル。
 */

#include "k2EngineLowPreCompile.h"
#include "Level.h"
#include "MapChip.h"

#include "level3D/TklFile.h"

namespace nsK2EngineLow {

	Level::~Level()
	{

	}
	void Level::CreateMapChip(const LevelObjectData& objData, const char* filePath)
	{
		//フックされなかったので、マップチップを作成する。
		auto mapChipRender = std::make_shared<MapChip>(objData, filePath);
		m_mapChipPtrs.push_back(mapChipRender);
	}

	void Level::Init(
		const char* filePath,
		std::function<bool(LevelObjectData& objData)> hookFunc
	)
	{
		//tklファイルをロードする。
		m_tklFile.Load(filePath);

		MatrixTklToLevel();

		//ルートボーンはいらないため、iを1から始めることで除外できる。
		for (int i = 1; i < m_bonelist.size(); i++) {
			auto bone = m_bonelist[i].get();
			if (bone->GetParentBoneNo() == 0) {

				//マップチップを作成する。
				//行列からポジションを作成する。
				LevelObjectData levelObjData;
				bone->CalcWorldTRS(levelObjData.position, levelObjData.rotation, levelObjData.scale);

				//ZupとYupの変更。
				float fix = levelObjData.position.y;
				levelObjData.position.y = levelObjData.position.z;
				levelObjData.position.z = -fix;

				fix = levelObjData.rotation.y;
				levelObjData.rotation.y = levelObjData.rotation.z;
				levelObjData.rotation.z = -fix;

				std::swap(levelObjData.scale.y, levelObjData.scale.z);
				//Zup⇔Yup　終わり。

				levelObjData.number = m_bonelist[i]->GetNo();

				levelObjData.name = m_bonelist[i]->GetName();

				//パスの作成
				wchar_t filePath[256];
				swprintf_s(filePath, L"Assets/modelData/%s.tkm", levelObjData.name);
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
				char* cFilePath = new char[newsize + strConSize];
				//マルチバイトに変換する。入りきらないものは切りすて
				wcstombs_s(&convertedChars, cFilePath, newsize, filePath, _TRUNCATE);
				//文字列の連結
				_mbscat_s((unsigned char*)cFilePath, newsize + strConSize, (unsigned char*)strCon);




				//Hookが登録済みならばマップチップは作成不要
				//false のままなら作成する。
				bool isHooked = false;
				//hookFuncになにも入っていない＝Sobjectには何もない。
				if (hookFunc != nullptr) {
					isHooked = hookFunc(levelObjData);
				}

				//hookがfalseなままならば。
				if (isHooked == false) {
					//マップチップレンダーを作成。
					CreateMapChip(levelObjData, cFilePath);
				}

			}
		}
	}

	void Level::MatrixTklToLevel()
	{
		m_tklFile.QuaryObject([&](TklFile::SObject& tklObj) {
			//コピー。
			//バインドポーズ。
			Matrix bindPoseMatrix;
			memcpy(bindPoseMatrix.m[0], &tklObj.bindPose[0], sizeof(tklObj.bindPose[0]));
			memcpy(bindPoseMatrix.m[1], &tklObj.bindPose[1], sizeof(tklObj.bindPose[1]));
			memcpy(bindPoseMatrix.m[2], &tklObj.bindPose[2], sizeof(tklObj.bindPose[2]));
			memcpy(bindPoseMatrix.m[3], &tklObj.bindPose[3], sizeof(tklObj.bindPose[3]));
			bindPoseMatrix.m[0][3] = 0.0f;
			bindPoseMatrix.m[1][3] = 0.0f;
			bindPoseMatrix.m[2][3] = 0.0f;
			bindPoseMatrix.m[3][3] = 1.0f;

			/// <summary>
			/// バインドポーズの逆行列
			/// 列3はVector3変換の際に不要なので初期化。
			/// </summary>
			Matrix invBindPoseMatirx;
			memcpy(invBindPoseMatirx.m[0], &tklObj.invBindPose[0], sizeof(tklObj.invBindPose[0]));
			memcpy(invBindPoseMatirx.m[1], &tklObj.invBindPose[1], sizeof(tklObj.invBindPose[1]));
			memcpy(invBindPoseMatirx.m[2], &tklObj.invBindPose[2], sizeof(tklObj.invBindPose[2]));
			memcpy(invBindPoseMatirx.m[3], &tklObj.invBindPose[3], sizeof(tklObj.invBindPose[3]));
			invBindPoseMatirx.m[0][3] = 0.0f;
			invBindPoseMatirx.m[1][3] = 0.0f;
			invBindPoseMatirx.m[2][3] = 0.0f;
			invBindPoseMatirx.m[3][3] = 1.0f;

			wchar_t boneName[256];

			/// <summary>
			/// tklのオブジェクト名の型をワイド文字に変換。
			/// char -> wchar_t
			/// 変換するchar型の文字列。
			/// 変換結果のwchar_t型文字列。
			/// サイズ
			/// </summary>
			mbstowcs(boneName, tklObj.name.get(), 256);
			BonePtr bone = std::make_unique<Bone>(
				boneName,
				bindPoseMatrix,
				invBindPoseMatirx,
				tklObj.parentNo,
				tklObj.no
				);
			//ボーンを積む。
			m_bonelist.push_back(std::move(bone));
			//コピー終わり。
		});

	}

	void Level::Draw(RenderContext& rc)
	{
		for (auto mapChipRenderPtr : m_mapChipPtrs)
		{
			mapChipRenderPtr->Draw(rc);
		}
	}
}