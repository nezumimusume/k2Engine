#pragma once
#include "online/SyncOnlineTwoPlayerMatchEngine.h"

class Actor;
class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// �����o�֐��B
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	/// <summary>
	/// ���ׂẴv���C���[�����[���ɃW���C�������Ƃ��ɌĂ΂�鏈���B
	/// </summary>
	/// <param name="pData"></param>
	/// <param name="size"></param>
	void OnAllPlayerJoined(void* pData, int size);
	/// <summary>
	/// ���ׂẴv���C���[���Q�[���X�^�[�g�������ɌĂ΂�鏈���B
	/// </summary>
	void OnAllPlayerStartGame();
	/// <summary>
	/// �ʐM�G���[���N�����B
	/// </summary>
	void OnError();
private:
	//////////////////////////////////////
	// �����o�ϐ��B
	//////////////////////////////////////
	enum EnStep {
		enStep_CharacterSelect,			// �L�����N�^�[�Z���N�g�B
		enStep_WaitAllPlayerJoined,		// ���ׂẴv���C���[�������܂ő҂B
		enStep_WaitAllPlayerStartGame,	// ���ׂẴv���C���[���Q�[���J�n����̂�҂B
		enStep_InGame,					// �C���Q�[���B
		enStep_Error,					// �G���[�B
	};
	FontRender m_fontRender;
	FontRender m_positionRender[2];
	int m_charaNo = -1;
	EnStep m_step = enStep_CharacterSelect;
	Actor* m_actor[2];
	SyncOnlineTwoPlayerMatchEngine* m_onlineTwoPlayerMatchEngine = nullptr; // ���S������l�ΐ�p�G���W���B
};

