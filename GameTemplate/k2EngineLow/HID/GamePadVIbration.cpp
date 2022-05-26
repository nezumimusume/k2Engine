/*!
*@brief	パッド。
*/
#include "k2EngineLowPreCompile.h"
#include "GamePadVibration.h"


namespace nsK2EngineLow {
	void GamePadVibration::Init(int padNo, float durationSec, float normalizedPower)
	{
		m_xinputVibration.wLeftMotorSpeed = static_cast<WORD>(UINT16_MAX * normalizedPower);
		m_xinputVibration.wRightMotorSpeed = static_cast<WORD>(UINT16_MAX * normalizedPower);
		m_padNo = padNo;
		XInputSetState(padNo, &m_xinputVibration);
		m_durationSec = durationSec;
	}
	bool GamePadVibration::Start()
	{
		return true;
	}
	void GamePadVibration::Update()
	{
		m_timerSec += g_gameTime->GetFrameDeltaTime();
		if (m_timerSec > m_durationSec) {
			// 終わり。
			m_xinputVibration.wLeftMotorSpeed = 0;
			m_xinputVibration.wRightMotorSpeed = 0;
			XInputSetState(m_padNo, &m_xinputVibration);
			DeleteGO(this);
		}
	}
}
