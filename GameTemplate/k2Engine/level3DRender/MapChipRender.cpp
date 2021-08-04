/*!
 *@brief	�}�b�v�`�b�v
 */
#include "k2EnginePreCompile.h"
#include "MapChipRender.h"
#include "LevelRender.h"


MapChipRender::MapChipRender(const LevelObjectData& objData, const char* filePath)
{
	//���f���̃t�@�C���p�X���擾�B
	m_filePath = std::make_unique<const char*>(filePath);

	//�}�b�v�`�b�v�f�[�^��ǉ�����B
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
	//�C���X�^���V���O�`��p�Ƀ��f�����������B
	m_modelRender.Init(*m_filePath.get(), nullptr, 0, enModelUpAxisZ, true, static_cast<int>(m_mapChipDataVector.size()));

	for (auto& mapChipData : m_mapChipDataVector)
	{
		//���[���h�s����v�Z����B
		Matrix worldMatrix =  m_modelRender.GetModel().CalcWorldMatrix(mapChipData.position, mapChipData.rotation, mapChipData.scale);
		auto p = std::make_unique<PhysicsStaticObject>();
		//�ÓI�����I�u�W�F�N�g���쐬�B
		p->CreateFromModel(m_modelRender.GetModel(), worldMatrix);
		m_physicsStaticObjectPtrVector.push_back(std::move(p));
	}
}

void MapChipRender::Update()
{
	for (auto& mapChipData : m_mapChipDataVector)
	{
		//���f�������_�[�̃C���X�^���V���O�p�̃f�[�^���X�V�B
		m_modelRender.UpdateInstancingData(mapChipData.position, mapChipData.rotation, mapChipData.scale);
	}
}

void MapChipRender::Draw(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}

