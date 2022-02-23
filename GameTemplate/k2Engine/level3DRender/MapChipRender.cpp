/*!
 *@brief	�}�b�v�`�b�v
 */
#include "k2EnginePreCompile.h"
#include "MapChipRender.h"
#include "LevelRender.h"

namespace nsK2Engine {

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
		m_mapChipDataNum = static_cast<int>(m_mapChipDataVector.size());
		if (m_mapChipDataNum == 1)
		{
			m_modelRender.Init(*m_filePath.get());
			auto& mapChipData = m_mapChipDataVector[0];
			m_modelRender.SetTRS(mapChipData.position, mapChipData.rotation, mapChipData.scale);
			m_modelRender.Update();
			auto p = std::make_unique<PhysicsStaticObject>();
			//�ÓI�����I�u�W�F�N�g���쐬�B
			p->CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
			m_physicsStaticObjectPtrVector.push_back(std::move(p));
			return;
		}

		//�C���X�^���V���O�`��p�Ƀ��f�����������B
		m_modelRender.Init(*m_filePath.get(), nullptr, 0, enModelUpAxisZ, true, m_mapChipDataNum);

		for (auto& mapChipData : m_mapChipDataVector)
		{
			//���[���h�s����v�Z����B
			Matrix worldMatrix = m_modelRender.GetModel().CalcWorldMatrix(mapChipData.position, mapChipData.rotation, mapChipData.scale);
			auto p = std::make_unique<PhysicsStaticObject>();
			//�ÓI�����I�u�W�F�N�g���쐬�B
			p->CreateFromModel(m_modelRender.GetModel(), worldMatrix);
			m_physicsStaticObjectPtrVector.push_back(std::move(p));
		}
	}

	void MapChipRender::Update()
	{
		if (m_mapChipDataNum == 1)
		{
			m_modelRender.Update();
		}
		else
		{
			int instanceNo = 0;
			for (auto& mapChipData : m_mapChipDataVector)
			{
				//���f�������_�[�̃C���X�^���V���O�p�̃f�[�^���X�V�B
				m_modelRender.UpdateInstancingData(instanceNo, mapChipData.position, mapChipData.rotation, mapChipData.scale);
				instanceNo++;
			}
		}
	}

	void MapChipRender::Draw(RenderContext& rc)
	{
		m_modelRender.Draw(rc);
	}
}
