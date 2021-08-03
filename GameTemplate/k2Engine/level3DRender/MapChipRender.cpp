/*!
 *@brief	マップチップ
 */
#include "k2EnginePreCompile.h"
#include "MapChipRender.h"
#include "LevelRender.h"


MapChipRender::MapChipRender(const LevelObjectData& objData, const char* filePath)
{
	//モデルのファイルパスを取得。
	m_filePath = std::make_unique<const char*>(filePath);

	//マップチップデータを追加する。
	AddMapChipData(objData);
}

void MapChipRender::AddMapChipData(const LevelObjectData& objData)
{
	MapChipData mapChipData;
	mapChipData.position = objData.position;
	mapChipData.rotation = objData.rotation;
	mapChipData.scale = objData.scale;
	m_mapChipDataVector.push_back(mapChipData);
}

void MapChipRender::Init()
{
	//インスタンシング描画用にモデルを初期化。
	m_modelRender.Init(*m_filePath.get(), nullptr, 0, enModelUpAxisZ, true, static_cast<int>(m_mapChipDataVector.size()));

	for (auto& mapChipData : m_mapChipDataVector)
	{
		//ワールド行列を計算する。
		Matrix worldMatrix =  m_modelRender.GetModel().CalcWorldMatrix(mapChipData.position, mapChipData.rotation, mapChipData.scale);
		auto p = std::make_unique<PhysicsStaticObject>();
		//静的物理オブジェクトを作成。
		p->CreateFromModel(m_modelRender.GetModel(), worldMatrix);
		m_physicsStaticObjectPtrVector.push_back(std::move(p));
	}
}

void MapChipRender::Update()
{
	for (auto& mapChipData : m_mapChipDataVector)
	{
		//モデルレンダーのインスタンシング用のデータを更新。
		m_modelRender.UpdateInstancingData(mapChipData.position, mapChipData.rotation, mapChipData.scale);
	}
}

void MapChipRender::Draw(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

