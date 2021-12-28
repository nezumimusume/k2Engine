#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "GameCamera.h"
#include "Background.h"
#include "Enemy.h"
#include "Lever.h"
#include "Door.h"
#include "Boss.h"
#include "Fade.h"
#include "Title.h"


Game::Game()
{

}

Game::~Game()
{
	//�F�X�ȃI�u�W�F�N�g���폜����B
	DeleteGO(m_gameCamera);
	DeleteGO(m_skyCube);
	DeleteGO(m_player);
	DeleteGO(m_background);
	for (auto lever : m_leverVector)
	{
		DeleteGO(lever);
	}
	for (auto door : m_doorVector)
	{
		DeleteGO(door);
	}
	const auto& enemys = FindGOs<Enemy>("enemy");
	for (auto enemy : enemys)
	{
		DeleteGO(enemy);
	}
	DeleteGO(FindGO<Boss>("boss"));
	DeleteGO(m_bgm);
}

bool Game::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, -600.0f });
	g_camera3D->SetFar(40000.0f);

	//�J�����̃I�u�W�F�N�g���쐬����B
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	//��̃I�u�W�F�N�g���쐬����B
	m_skyCube = NewGO<SkyCube>(0, "skycube");
	g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 0.1f);
	g_renderingEngine->SetCascadeNearAreaRates(0.05f, 0.1f, 1.0f);
	//���x�����\�z����B
	m_levelRender.Init("Assets/level3D/level.tkl", [&](LevelObjectData& objData) {
		//���O��human��������B
		if (objData.EqualObjectName(L"human") == true) {
			//�v���C���[�̃I�u�W�F�N�g���쐬����B
			m_player = NewGO<Player>(0, "player");
			//���W��ݒ肷��B
			m_player->SetPosition(objData.position);
			//��]��ݒ肷��B
			m_player->SetRotation(objData.rotation);
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		//���O��enemy��������B
		else if (objData.ForwardMatchName(L"enemy") == true) {
			//�G�l�~�[�̃I�u�W�F�N�g���쐬����B
			Enemy* enemy = NewGO<Enemy>(0, "enemy");
			//���W��ݒ肷��B
			enemy->SetPosition(objData.position);
			//��]��ݒ肷��B
			enemy->SetRotation(objData.rotation);
			//HP��5�ɐݒ肷��B
			enemy->SetHP(5);
			//�쐬�����G�l�~�[�̐��𐔂������̂ŁA+1����B
			m_numEnemy++;
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		//���O��boss��������B
		else if (objData.ForwardMatchName(L"boss") == true) {
			//�{�X�̃I�u�W�F�N�g���쐬����B
			Boss* boss = NewGO<Boss>(0, "boss");
			//���W��ݒ肷��B
			boss->SetPosition(objData.position);
			//��]��ݒ肷��B
			boss->SetRotation(objData.rotation);
			//�傫����ݒ肷��B
			boss->SetScale(objData.scale);
			//HP��15�ɐݒ肷��B
			boss->SetHP(15);
			//�쐬�����G�l�~�[�̐��𐔂������̂ŁA+1����B
			m_numEnemy++;
			//true�ɂ���ƁA���x���̕��Ń��f�����ǂݍ��܂�Ȃ��B
			return true;
		}
		//���O��background��������B
		else if (objData.EqualObjectName(L"background") == true) {
			//�w�i�I�u�W�F�N�g���쐬����B
			m_background = NewGO<Background>(0, "background");
			//���W��ݒ肷��B
			m_background->SetPosition(objData.position);
			//�傫����ݒ肷��B
			m_background->SetScale(objData.scale);
			//��]��ݒ肷��B
			m_background->SetRotation(objData.rotation);
			return true;
		}
		//���O��lever��������B
		else if (objData.ForwardMatchName(L"lever") == true) {
			//���o�[�̃I�u�W�F�N�g���쐬����B
			auto lever = NewGO<Lever>(0, "lever");
			//���W��ݒ肷��B
			lever->SetPosition(objData.position);
			//�傫����ݒ肷��B
			lever->SetScale(objData.scale);
			//��]��ݒ肷��B
			lever->SetRotation(objData.rotation);
			//�ԍ���ݒ肷��B
			lever->SetLeverNumber(objData.number);
			//���o�[�̔z��ɍ쐬�����I�u�W�F�N�g��������B
			m_leverVector.push_back(lever);
			return true;
		}
		//���O��door��������B
		else if (objData.ForwardMatchName(L"door") == true) {
			//�h�A�I�u�W�F�N�g���쐬����B
			auto door = NewGO<Door>(0, "door");
			//���W��ݒ肷��B
			door->SetPosition(objData.position);
			//�傫����ݒ肷��B
			door->SetScale(objData.scale);
			//��]��ݒ肷��B
			door->SetRotation(objData.rotation);
			//�ԍ���ݒ肷��B
			door->SetDoorNumber(objData.number);
			//�h�A�̔z��ɍ쐬�����I�u�W�F�N�g��������B
			m_doorVector.push_back(door);
			return true;
		}
		return true;
		});

	//�����蔻���L��������B
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//����ǂݍ��ށB
	g_soundEngine->ResistWaveFileBank(2, "Assets/sound/game.wav");
	//BGM���Đ�����B
	m_bgm = NewGO<SoundSource>(0);
	m_bgm->Init(2);
	m_bgm->Play(true);
	m_bgm->SetVolume(0.2f);

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();
	return true;
}

void Game::NotifyGameClear()
{
	m_isWaitFadeout = true;
	m_fade->StartFadeOut();
}

void Game::NotifyGameOver()
{
	m_isWaitFadeout = true;
	m_fade->StartFadeOut();
}

void Game::Update()
{
	if (m_isWaitFadeout) {
		if (!m_fade->IsFade()) {
			NewGO<Title>(0, "title");
			DeleteGO(this);
		}
	}
}