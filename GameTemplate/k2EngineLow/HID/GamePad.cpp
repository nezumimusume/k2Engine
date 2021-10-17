/*!
*@brief	�p�b�h�B
*/
#include "k2EngineLowPreCompile.h"
#include "GamePad.h"
#include <limits.h>

namespace nsK2EngineLow {
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
	std::array<GamePad*, GamePad::CONNECT_PAD_MAX> g_pad;

	namespace {
		/*!
		*@brief	���z�{�^����XBox�R���g���[���̃{�^���Ƃ̊֘A�t����\���\���́B
		*/
		struct VirtualPadToXPad {
			EnButton vButton;		//!<���z�{�^���B
			DWORD	 xButton;		//!<XBox�R���g���[���̃{�^���B
		};

		const VirtualPadToXPad vPadToXPadTable[enButtonNum] = {
			{ enButtonUp		, XINPUT_GAMEPAD_DPAD_UP },
			{ enButtonDown		, XINPUT_GAMEPAD_DPAD_DOWN },
			{ enButtonLeft		, XINPUT_GAMEPAD_DPAD_LEFT },
			{ enButtonRight		, XINPUT_GAMEPAD_DPAD_RIGHT },
			{ enButtonA			, XINPUT_GAMEPAD_A },
			{ enButtonB			, XINPUT_GAMEPAD_B },
			{ enButtonY			, XINPUT_GAMEPAD_Y },
			{ enButtonX			, XINPUT_GAMEPAD_X },
			{ enButtonSelect	, XINPUT_GAMEPAD_BACK },
			{ enButtonStart		, XINPUT_GAMEPAD_START },
			{ enButtonRB1		, XINPUT_GAMEPAD_RIGHT_SHOULDER },
			{ enButtonRB2		, 0 },
			{ enButtonRB3		, XINPUT_GAMEPAD_RIGHT_THUMB },
			{ enButtonLB1		, XINPUT_GAMEPAD_LEFT_SHOULDER },
			{ enButtonLB2		, 0 },
			{ enButtonLB3		, XINPUT_GAMEPAD_LEFT_THUMB },
		};
		/*!
		*@brief	���z�{�^���ƃL�[�{�[�h�Ƃ̊֘A�t����\���\���́B
		*/
		struct VirtualPadToKeyboard {
			EnButton vButton;		//!<���z�{�^��
			DWORD keyCoord1;		//!<�L�[�{�[�h�̃L�[�R�[�h�B
			DWORD keyCoord2;		//!<�L�[�{�[�h�̃L�[�R�[�h�B
		};
		const VirtualPadToKeyboard vPadToKeyboardTable[enButtonNum] = {
			{ enButtonUp		, VK_NUMPAD8,	'8' 		},
			{ enButtonDown		, VK_NUMPAD2,	'2' 		},
			{ enButtonLeft		, VK_NUMPAD4,	'4' 		},
			{ enButtonRight		, VK_NUMPAD6,	'6' 		},
			{ enButtonA			, 'J',			0xFFFFFFFF	},
			{ enButtonB			, 'K',			0xFFFFFFFF	},
			{ enButtonY			, 'I',			0xFFFFFFFF	},
			{ enButtonX			, 'L',			0xFFFFFFFF	},
			{ enButtonSelect	, VK_SPACE,		0xFFFFFFFF	},
			{ enButtonStart		, VK_RETURN,	0xFFFFFFFF	},
			{ enButtonRB1		, '7',			0xFFFFFFFF	},
			{ enButtonRB2		, '8',			0xFFFFFFFF	},
			{ enButtonRB3		, '9',			0xFFFFFFFF	},
			{ enButtonLB1		, 'B',			0xFFFFFFFF	},
			{ enButtonLB2		, 'N',			0xFFFFFFFF	},
			{ enButtonLB3		, 'M',			0xFFFFFFFF	},
		};
	}
	GamePad::EnXInputPadState GamePad::m_padStates[4] = { GamePad::EnXInputPadState::Undef };
	GamePad::GamePad()
	{
		memset(&m_state, 0, sizeof(m_state));
		memset(m_trigger, 0, sizeof(m_trigger));
		memset(m_press, 0, sizeof(m_press));
	}
	GamePad::~GamePad()
	{
	}
	void GamePad::BeginFrame()
	{
		//�S�Ė���`�ɂ���B
		for (auto& padState : m_padStates) {
			padState = EnXInputPadState::Undef;
		}
	}
	void GamePad::UpdateCore(XINPUT_STATE xInputState)
	{
		for (const VirtualPadToXPad& vPadToXPad : vPadToXPadTable) {
			if (xInputState.Gamepad.wButtons & vPadToXPad.xButton) {
				m_trigger[vPadToXPad.vButton] = 1 ^ m_press[vPadToXPad.vButton];
				m_press[vPadToXPad.vButton] = 1;
			}
			else {
				m_trigger[vPadToXPad.vButton] = 0;
				m_press[vPadToXPad.vButton] = 0;
			}
		}
		//���g���K�[�B
		if (xInputState.Gamepad.bLeftTrigger != 0) {
			m_trigger[enButtonLB2] = 1 ^ m_press[enButtonLB2];
			m_press[enButtonLB2] = 1;
		}
		else {
			m_trigger[enButtonLB2] = 0;
			m_press[enButtonLB2] = 0;
		}
		//�E�g���K�[
		if (xInputState.Gamepad.bRightTrigger != 0) {
			m_trigger[enButtonRB2] = 1 ^ m_press[enButtonRB2];
			m_press[enButtonRB2] = 1;
		}
		else {
			m_trigger[enButtonRB2] = 0;
			m_press[enButtonRB2] = 0;
		}
		if ((xInputState.Gamepad.sThumbLX < INPUT_DEADZONE &&
			xInputState.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
			(xInputState.Gamepad.sThumbLY < INPUT_DEADZONE &&
				xInputState.Gamepad.sThumbLY > -INPUT_DEADZONE))
		{
			xInputState.Gamepad.sThumbLX = 0;
			xInputState.Gamepad.sThumbLY = 0;
			m_lStickX = 0.0f;
			m_lStickY = 0.0f;
		}
		else {
			//���X�e�B�b�N�̓��͗ʁB
			if (xInputState.Gamepad.sThumbLX > 0) {
				m_lStickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / SHRT_MAX;
			}
			else {
				m_lStickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / -SHRT_MIN;
			}
			if (xInputState.Gamepad.sThumbLY > 0) {
				m_lStickY = static_cast<float>(xInputState.Gamepad.sThumbLY) / SHRT_MAX;
			}
			else {
				m_lStickY = static_cast<float>(xInputState.Gamepad.sThumbLY) / -SHRT_MIN;
			}

			// ���͗ʂ𐳋K������B
			float t = sqrtf(m_lStickX * m_lStickX + m_lStickY * m_lStickY);
			if (t > 0.001f) {
				m_lStickX /= t;
				m_lStickY /= t;
			}
		}

		if ((xInputState.Gamepad.sThumbRX < INPUT_DEADZONE &&
			xInputState.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
			(xInputState.Gamepad.sThumbRY < INPUT_DEADZONE &&
				xInputState.Gamepad.sThumbRY > -INPUT_DEADZONE))
		{
			xInputState.Gamepad.sThumbRX = 0;
			xInputState.Gamepad.sThumbRY = 0;
			m_rStickX = 0.0f;
			m_rStickY = 0.0f;
		}
		else {
			//�E�X�e�B�b�N�̓��͗ʁB
			if (xInputState.Gamepad.sThumbRX > 0) {
				m_rStickX = static_cast<float>(xInputState.Gamepad.sThumbRX) / SHRT_MAX;
			}
			else {
				m_rStickX = static_cast<float>(xInputState.Gamepad.sThumbRX) / -SHRT_MIN;
			}
			if (xInputState.Gamepad.sThumbRY > 0) {
				m_rStickY = static_cast<float>(xInputState.Gamepad.sThumbRY) / SHRT_MAX;
			}
			else {
				m_rStickY = static_cast<float>(xInputState.Gamepad.sThumbRY) / -SHRT_MIN;
			}
			// ���͗ʂ𐳋K������B
			float t = sqrtf(m_rStickX * m_rStickX + m_rStickY * m_rStickY);
			if (t > 0.001f) {
				m_rStickX /= t;
				m_rStickY /= t;
			}
		}
	}
	void GamePad::Update(const XINPUT_STATE& xInputState)
	{
		m_state.bConnected = true;
		UpdateCore(xInputState);
	}
	void GamePad::Update()
	{
		//�A�N�e�B�u�p�b�h��T���B
		DWORD result = ERROR_DEVICE_NOT_CONNECTED;
		for (int i = m_padNo; i < MAX_PAD; i++) {
			if (m_padStates[i] == EnXInputPadState::Undef) {
				//���̃p�b�h�͖������B
				result = XInputGetState(i, &m_state.state);
				if (result == ERROR_SUCCESS) {
					//�ڑ��ł����B
					m_padStates[i] = EnXInputPadState::Connect;
					break;
				}
				else {
					//�q�����Ă��Ȃ��B
					//���̃p�b�h�𒲂ׂ�B
					m_padStates[i] = EnXInputPadState::Disconnect;
				}
			}
		}

		if (result == ERROR_SUCCESS) {
			//�ڑ�����Ă���B
			m_state.bConnected = true;
			UpdateCore(m_state.state);
			
		}
		else {
			//�ڑ�����Ă��Ȃ��ꍇ�̓L�[�{�[�h�̓��͂ŃG�~�����[�g����B
			if (m_state.bConnected) {
				//���ڑ��ɂȂ����B
				memset(&m_state, 0, sizeof(m_state));
				memset(m_trigger, 0, sizeof(m_trigger));
				memset(m_press, 0, sizeof(m_press));
			}
			m_lStickX = 0.0f;
			m_lStickY = 0.0f;
			m_rStickX = 0.0f;
			m_rStickY = 0.0f;

			if (GetAsyncKeyState(VK_LEFT)) {
				m_rStickX = -1.0f;
			}
			else if (GetAsyncKeyState(VK_RIGHT)) {
				m_rStickX = 1.0f;
			}
			if (GetAsyncKeyState(VK_UP)) {
				m_rStickY = 1.0f;
			}
			else if (GetAsyncKeyState(VK_DOWN)) {
				m_rStickY = -1.0f;
			}
			//�X�e�B�b�N�̓��͗ʂ𐳋K���B
			float t = fabsf(m_rStickX) + fabsf(m_rStickY);
			if (t > 0.0f) {
				m_rStickX /= t;
				m_rStickY /= t;
			}

			if (GetAsyncKeyState('A')) {
				m_lStickX = -1.0f;
			}
			else if (GetAsyncKeyState('D')) {
				m_lStickX = 1.0f;
			}
			if (GetAsyncKeyState('W')) {
				m_lStickY = 1.0f;
			}
			else if (GetAsyncKeyState('S')) {
				m_lStickY = -1.0f;
			}
			//�X�e�B�b�N�̓��͗ʂ𐳋K���B
			t = fabsf(m_lStickX) + fabsf(m_lStickY);
			if (t > 0.0f) {
				m_lStickX /= t;
				m_lStickY /= t;
			}

			for (const VirtualPadToKeyboard& vPadToKeyboard : vPadToKeyboardTable) {
				if (GetAsyncKeyState(vPadToKeyboard.keyCoord1)) {
					//�D�揇�ʂP�Ԗڂ̃L�[�B
					m_trigger[vPadToKeyboard.vButton] = 1 ^ m_press[vPadToKeyboard.vButton];
					m_press[vPadToKeyboard.vButton] = 1;
				}
				else if (vPadToKeyboard.keyCoord2 != 0xFFFFFFFF
					&& GetAsyncKeyState(vPadToKeyboard.keyCoord2)) {
					//�D�揇�ʂQ�Ԗڂ̃L�[�B
					m_trigger[vPadToKeyboard.vButton] = 1 ^ m_press[vPadToKeyboard.vButton];
					m_press[vPadToKeyboard.vButton] = 1;
				}
				else {
					m_trigger[vPadToKeyboard.vButton] = 0;
					m_press[vPadToKeyboard.vButton] = 0;
				}
			}
		}
	}
}
