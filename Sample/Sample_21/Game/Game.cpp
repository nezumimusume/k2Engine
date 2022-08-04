#include "stdafx.h"
#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

bool Game::Start()
{
	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/unityChan.tkm");

	//���x�����\�z����B
	m_level2DRender.Init("Assets/level2D/sample.casl", [&](Level2DObjectData& objData) {
		//���O����v���Ă�����B
		if (objData.EqualObjectName("k2SLEditor_red") == true) {


			//return false�ɂ���ƁALevel2D���ŉ摜���ǂݍ��܂�܂��B
			return false;
		}
		else if (objData.EqualObjectName("k2SLEditor_blue") == true) {

			//���x���̃f�[�^���g�p���ĉ摜��ǂݍ��ށB
			m_spriteRender.Init(objData.ddsFilePath, objData.width, objData.height);
			m_spriteRender.SetScale(objData.scale);
			m_spriteRender.SetPosition(objData.position);
			m_spriteRender.SetPivot(objData.pivot);
			//return true�ɂ���ƁALevel2D���ŉ摜���ǂݍ��܂�܂��B
			//���g�ŉ摜��ǂݍ��݂������͕K��true�ɂ��Ă��������B
			return true;
		}
		return false;
		});

	return true;
}

void Game::Update()
{
	//���f���̍X�V�B
	m_modelRender.Update();

	//���x��2D���œǂݍ��񂾉摜�̍X�V�B
	m_level2DRender.Update();

	//�摜�̍X�V�B
	m_spriteRender.Update();
}

void Game::Render(RenderContext& rc)
{
	//���f���̕`��B
	m_modelRender.Draw(rc);

	//���x��2D���œǂݍ��񂾉摜�̕`�揈���B
	m_level2DRender.Draw(rc);

	//�摜�̕`��B
	m_spriteRender.Draw(rc);
}