

#pragma once

#include "gameObject/GameObjectManager.h"
#include "gameObject/IGameObject.h"

namespace nsK2EngineLow {	
	class GamePadVibration : public IGameObject
	{
	public:
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="padNo">�p�b�h�ԍ�</param>
		/// <param name="durationSec">�o�C�u���[�V�������p�������鎞��(�P��:�b)</param>
		/// <param name="normalizedPower">�o�C�u���[�V�����̐��K�����ꂽ�́B</param>
		void Init(int padNo, float durationSec, float normalizedPower);
		bool Start() override;
		void Update() override;
	private:
		float m_durationSec = 0;				// �o�C�u���[�V�������p�����鎞��(�P��:�b)�B
		float m_timerSec = 0;					// �^�C�}�[(�P��:�b)�B
		int m_padNo = 0;						// �p�b�h�ԍ��B
		XINPUT_VIBRATION m_xinputVibration;		// XINPUT�ɓn���o�C�u���[�V�����̏��B
	};
}