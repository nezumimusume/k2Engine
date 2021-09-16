#include "stdafx.h"
#include "Actor.h"

void Actor::Init(
	GamePad& gamePad, 
	const char* filePath, 
	const Vector3& initPos,
	const Quaternion& initRot
)
{
	/*enState_Idle,
		enState_Walk,
		enState_Attack,
		enState_Down,
		enState_RecieveDamage,*/
	m_animClip[enAnimClip_Idle].Load("Assets/animData/human/idle.tka");
	m_animClip[enAnimClip_Walk].Load("Assets/animData/human/walk.tka");
	m_animClip[enAnimClip_Attack].Load("Assets/animData/human/attack.tka");
	m_animClip[enAnimClip_Down].Load("Assets/animData/human/down.tka");
	m_animClip[enAnimClip_ReceiveDamage].Load("Assets/animData/human/receivedamage.tka");

	m_animClip[enState_Idle].SetLoopFlag(true);
	m_animClip[enState_Walk].SetLoopFlag(true);
	m_animClip[enState_RecieveDamage].SetLoopFlag(true);
	m_animClip[enAnimClip_Attack].SetLoopFlag(false);
	m_animClip[enAnimClip_ReceiveDamage].SetLoopFlag(false);
	m_animClip[enAnimClip_Down].SetLoopFlag(false);

	m_gamePad = &gamePad;
	m_modelRender.Init(filePath, m_animClip, enAnimClip_Num);
	m_modelRender.PlayAnimation(enAnimClip_Idle);
	m_position = initPos;
	m_rotation = initRot;
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
}
void Actor::Update()
{
	switch (m_state) {
	case enState_Idle:
		if (m_gamePad->IsPress(enButtonA)) {
			// UŒ‚ó‘Ô‚É‘JˆÚB
			m_state = enState_Attack;
			m_modelRender.PlayAnimation(enAnimClip_Attack, 0.2f);
		}
		else {
			if (fabsf(m_gamePad->GetLStickXF()) > 0.0f) {
				// X
				m_modelRender.PlayAnimation(enAnimClip_Walk, 0.2f);
				m_state = enState_Walk;
			}
		}
		break;
	case enState_Walk:
		if (m_gamePad->IsPress(enButtonA)) {
			// UŒ‚ó‘Ô‚É‘JˆÚB
			m_state = enState_Attack;
			m_modelRender.PlayAnimation(enAnimClip_Attack, 0.2f);
		}
		else {
			m_position.x += m_gamePad->GetLStickXF() * 4.0f;
			if (fabsf(m_gamePad->GetLStickXF()) < 0.01f) {
				m_modelRender.PlayAnimation(enAnimClip_Idle, 0.2f);
				m_state = enState_Idle;
			}
		}
		break;
	case enState_Attack:
		if (m_modelRender.IsPlayingAnimation() == false) {
			// UŒ‚ƒ‚[ƒVƒ‡ƒ“‚ÌÄ¶I‚í‚èB
			m_modelRender.PlayAnimation(enAnimClip_Idle, 0.2f);
			m_state = enState_Idle;
		}
		break;
	}

	
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}
void Actor::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}