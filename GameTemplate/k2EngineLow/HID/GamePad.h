/*!
*@brief	�p�b�h
*/

#pragma once

namespace nsK2EngineLow {
	/*!
		*@brief	���z�{�^����`�B
		*/
	enum EnButton {
		enButtonUp,		//!<��B
		enButtonDown,		//!<���B
		enButtonLeft,		//!<���B
		enButtonRight,		//!<�E�B
		enButtonA,			//!<A�{�^���B
		enButtonB,			//!<B�{�^���B
		enButtonX,			//!<X�{�^���B
		enButtonY,			//!<Y�{�^���B
		enButtonSelect,	//!<�Z���N�g�{�^���B
		enButtonStart,		//!<�X�^�[�g�{�^���B
		enButtonRB1,		//!<RB1�{�^���B
		enButtonRB2,		//!<RB2�{�^���B
		enButtonRB3,		//!<RB3�{�^���B
		enButtonLB1,		//!<LB1�{�^���B
		enButtonLB2,		//!<LB2�{�^���B
		enButtonLB3,		//!<LB3�{�^���B
		enButtonNum,	//!<�{�^���̐��B
	};
	/*!
	*@brief	�p�b�h
	*/
	class GamePad : public Noncopyable {
	public:
		static const int CONNECT_PAD_MAX = 4;		//�ڑ��\�ȃp�b�h�̍ő吔�B
		/*!
			*@brief	�p�b�h�X�e�[�g�B
			*/
		struct PAD_STATE
		{
			XINPUT_STATE state;
			bool bConnected;
		};
		/*!
			*@brief	�R���X�g���N�^�B
			*/
		GamePad();
		/*!
			*@brief	�f�X�g���N�^�B
			*/
		~GamePad();
		/*!
		*@brief	�������B
		*@param[in]	padNo	�p�b�h�ԍ��B
		*/
		void Init(int padNo)
		{
			m_padNo = padNo;
		}
		/*!
		*@brief	�X�V�B
		*/
		void Update();

		/// <summary>
		/// �X�V(�O������L�[���͂�n���o�[�W����)
		/// </summary>
		/// <param name="xInputState">�L�[����</param>
		void Update(const XINPUT_STATE& xInputState);
		/*!
		*@brief	�{�^���̃g���K�[����B
		*@return	true���Ԃ��Ă�����g���K�[���́B
		*/
		bool IsTrigger(EnButton button) const
		{
			return m_trigger[button] != 0;
		}
		/*!
		*@brief	�{�^����������Ă��邩����B
		*@return	true���Ԃ��Ă����牟����Ă���B
		*/
		bool IsPress(EnButton button) const
		{
			return m_press[button] != 0;
		}
		/*!
		*@brief	�����̃{�^����������Ă��邩����
		*@return	true���Ԃ��Ă����牟����Ă���B
		*/
		bool IsPressAnyKey() const
		{
			return IsPress(enButtonUp)
				|| IsPress(enButtonDown)
				|| IsPress(enButtonLeft)
				|| IsPress(enButtonRight)
				|| IsPress(enButtonA)
				|| IsPress(enButtonB)
				|| IsPress(enButtonX)
				|| IsPress(enButtonY)
				|| IsPress(enButtonSelect)
				|| IsPress(enButtonStart)
				|| IsPress(enButtonRB1)
				|| IsPress(enButtonRB2)
				|| IsPress(enButtonRB3)
				|| IsPress(enButtonLB1)
				|| IsPress(enButtonLB2)
				|| IsPress(enButtonLB3);
		}
		/*!
		*@brief	���X�e�B�b�N��X���̓��͗ʂ��擾�B
		*@return	-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B
		*/
		float GetLStickXF() const
		{
			return m_lStickX;
		}
		/*!
		*@brief	���X�e�B�b�N��Y���̓��͗ʂ��擾�B
		*@return	-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B
		*/
		float GetLStickYF() const
		{
			return m_lStickY;
		}
		/*!
		*@brief	�E�X�e�B�b�N��X���̓��͗ʂ��擾�B
		*@return	-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B
		*/
		float GetRStickXF() const
		{
			return m_rStickX;
		}
		/*!
		*@brief	�E�X�e�B�b�N��Y���̓��͗ʂ��擾�B
		*@return	-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B
		*/
		float GetRStickYF() const
		{
			return m_rStickY;
		}
		//�t���[���̊J�n���ɌĂ΂��֐��B
		static void BeginFrame();
		/// <summary>
		/// XInput������������Ă������f�[�^���擾�B
		/// </summary>
		/// <returns></returns>
		const XINPUT_STATE& GetXInputState()
		{
			return m_state.state;
		}
	private:
		void UpdateCore(XINPUT_STATE xInputState);
	public:
		enum { MAX_PAD = 4 };
		enum class EnXInputPadState {
			Undef,		//�s���B
			Connect,	//�ڑ�����Ă���B
			Disconnect,	//�ڑ�����Ă��Ȃ��B
		};
	private:
		PAD_STATE m_state;	//!<�p�b�h�X�e�[�g�B
		int m_padNo = 0;			//!<�p�b�h�ԍ��B
		int m_trigger[enButtonNum];	//!<�g���K�[���͂̃t���O�B
		int m_press[enButtonNum];	//!<press���͂̃t���O�B
		float m_lStickX = 0.0f;		//!<���X�e�B�b�N��X���̓��͗ʁB
		float m_lStickY = 0.0f;		//!<���X�e�B�b�N��Y���̓��͗ʁB
		float m_rStickX = 0.0f;		//!<�E�X�e�B�b�N��X���̓��͗ʁB
		float m_rStickY = 0.0f;		//!<�E�X�e�B�b�N��Y���̓��͗ʁB

		static EnXInputPadState m_padStates[MAX_PAD];
	};

	extern std::array<GamePad*, GamePad::CONNECT_PAD_MAX> g_pad;
}