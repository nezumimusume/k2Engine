

#pragma once

namespace nsK2EngineLow {	
	/// <summary>
	/// ���z�{�^����`
	/// </summary>
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

	/// <summary>
	/// �Q�[���p�b�h�N���X
	/// </summary>
	class GamePad : public Noncopyable {
	public:
		static const int CONNECT_PAD_MAX = 4;		//�ڑ��\�ȃp�b�h�̍ő吔�B
		
		/// <summary>
		/// �p�b�h�X�e�[�^�X
		/// </summary>
		struct PAD_STATE
		{
			XINPUT_STATE state;
			bool bConnected;
		};
		

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		GamePad();
	
		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		~GamePad();

		/// <summary>
		/// ������
		/// </summary>
		/// <param name="padNo">�p�b�h�ԍ�</param>
		void Init(int padNo)
		{
			m_padNo = padNo;
		}
		/// <summary>
		/// �p�b�h���̍X�V
		/// </summary>
		void Update();

		/// <summary>
		/// �p�b�h���̍X�V(�O������L�[���͂�n���o�[�W����)
		/// </summary>
		/// <param name="xInputState">�L�[����</param>
		void Update(const XINPUT_STATE& xInputState);
		
		/// <summary>
		/// �{�^���̃g���K�[����
		/// </summary>
		/// <param name="button">���肵�����{�^��</param>
		/// <returns>true���Ԃ��Ă�����g���K�[���͂���Ă���</returns>
		bool IsTrigger(EnButton button) const
		{
			return m_trigger[button] != 0;
		}
		
		/// <summary>
		/// �{�^����������Ă��邩����
		/// </summary>
		/// <param name="button">���肵�����{�^��</param>
		/// <returns>true���Ԃ��Ă�������͂���Ă���B</returns>
		bool IsPress(EnButton button) const
		{
			return m_press[button] != 0;
		}

		/// <summary>
		/// �����̃{�^����������Ă��邩����B
		/// </summary>
		/// <returns>true���Ԃ��Ă����牟����Ă���B</returns>
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
		/// <summary>
		/// ���X�e�B�b�N��x���̓��͗ʂ��擾
		/// </summary>
		/// <returns>-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B</returns>
		float GetLStickXF() const
		{
			return m_lStickX;
		}
		
		/// <summary>
		/// ���X�e�B�b�N��Y���̓��͗ʂ��擾�B
		/// </summary>
		/// <returns>-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B</returns>
		float GetLStickYF() const
		{
			return m_lStickY;
		}
		

		/// <summary>
		/// �E�X�e�B�b�N��X���̓��͗ʂ��擾�B
		/// </summary>
		/// <returns>-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B</returns>
		float GetRStickXF() const
		{
			return m_rStickX;
		}
		
		/// <summary>
		/// �E�X�e�B�b�N��Y���̓��͗ʂ��擾
		/// </summary>
		/// <returns>-1.0�`1.0�̐��K�����ꂽ�l��Ԃ��B</returns>
		float GetRStickYF() const
		{
			return m_rStickY;
		}
		
		
		/// <summary>
		/// �t���[���̊J�n���ɌĂяo���K�v������֐�
		/// </summary>
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
		/// <summary>
		/// �ő�p�b�h��
		/// </summary>
		enum { MAX_PAD = 4 };

		/// <summary>
		/// XInput�p�b�h�̃X�e�[�g�B
		/// </summary>
		enum class EnXInputPadState {
			Undef,		//�s���B
			Connect,	//�ڑ�����Ă���B
			Disconnect,	//�ڑ�����Ă��Ȃ��B
		};
	private:
		PAD_STATE m_state;			// �p�b�h�X�e�[�g�B
		int m_padNo = 0;			// �p�b�h�ԍ��B
		int m_trigger[enButtonNum];	// �g���K�[���͂̃t���O�B
		int m_press[enButtonNum];	// press���͂̃t���O�B
		float m_lStickX = 0.0f;		// ���X�e�B�b�N��X���̓��͗ʁB
		float m_lStickY = 0.0f;		// ���X�e�B�b�N��Y���̓��͗ʁB
		float m_rStickX = 0.0f;		// �E�X�e�B�b�N��X���̓��͗ʁB
		float m_rStickY = 0.0f;		// �E�X�e�B�b�N��Y���̓��͗ʁB

		static EnXInputPadState m_padStates[MAX_PAD];
	};

	extern std::array<GamePad*, GamePad::CONNECT_PAD_MAX> g_pad;
}