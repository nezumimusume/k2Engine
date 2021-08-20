#include "stdafx.h"
#include "Player.h"

#include "Game.h"

#include "sound/SoundSource.h"
#include "sound/SoundEngine.h"


Player::Player()
{

}

Player::~Player()
{
	
}

bool Player::Start()
{
	
	//���j�e�B�����̃��f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.SetPosition({-80.0f, 0.0f, 0.0f});
	return true;
}

void Player::Update()
{
	//���f�����X�V�B
	m_modelRender.Update();
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}