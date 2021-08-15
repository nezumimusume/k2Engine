/*!
 * @brief	�G�t�F�N�g
 */

#include "k2EngineLowPreCompile.h"
#include "Effect.h"

namespace nsK2EngineLow {

	Effect::Effect()
	{
	}
	Effect::~Effect()
	{

	}


	void Effect::Init(const int number)
	{
		m_effect = EffectEngine::GetInstance()->LoadEffect(number);

	}
	void Effect::Play()
	{
		//�Đ����̃G�t�F�N�g���~����B
		EffectEngine::GetInstance()->Stop(m_handle);
		//�V�����Đ��B
		m_handle = EffectEngine::GetInstance()->Play(m_effect);
	}
	void Effect::Update()
	{
		EffectEngine::GetInstance()->UpdateEffectWorldMatrix(
			m_handle,
			m_position,
			m_rotation,
			m_scale
		);
	}
}