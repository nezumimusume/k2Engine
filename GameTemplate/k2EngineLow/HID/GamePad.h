

#pragma once

#include "HID/GamePadVibration.h"

namespace nsK2EngineLow {	
	/// <summary>
	/// 仮想ボタン定義
	/// </summary>
	enum EnButton {
		enButtonUp,		//!<上。
		enButtonDown,		//!<下。
		enButtonLeft,		//!<左。
		enButtonRight,		//!<右。
		enButtonA,			//!<Aボタン。
		enButtonB,			//!<Bボタン。
		enButtonX,			//!<Xボタン。
		enButtonY,			//!<Yボタン。
		enButtonSelect,	//!<セレクトボタン。
		enButtonStart,		//!<スタートボタン。
		enButtonRB1,		//!<RB1ボタン。
		enButtonRB2,		//!<RB2ボタン。
		enButtonRB3,		//!<RB3ボタン。
		enButtonLB1,		//!<LB1ボタン。
		enButtonLB2,		//!<LB2ボタン。
		enButtonLB3,		//!<LB3ボタン。
		enButtonNum,	//!<ボタンの数。
	};

	/// <summary>
	/// ゲームパッドクラス
	/// </summary>
	class GamePad : public Noncopyable {
	public:
		static const int CONNECT_PAD_MAX = 4;		//接続可能なパッドの最大数。
		
		/// <summary>
		/// パッドステータス
		/// </summary>
		struct PAD_STATE
		{
			XINPUT_STATE state;
			bool bConnected;
		};
		

		/// <summary>
		/// コンストラクタ
		/// </summary>
		GamePad();
	
		/// <summary>
		/// デストラクタ
		/// </summary>
		~GamePad();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="padNo">パッド番号</param>
		void Init(int padNo)
		{
			m_padNo = padNo;
		}
		/// <summary>
		/// パッド情報の更新
		/// </summary>
		void Update();

		/// <summary>
		/// パッド情報の更新(外部からキー入力を渡すバージョン)
		/// </summary>
		/// <param name="xInputState">キー入力</param>
		void Update(const XINPUT_STATE& xInputState);
		
		/// <summary>
		/// ボタンのトリガー判定
		/// </summary>
		/// <param name="button">判定したいボタン</param>
		/// <returns>trueが返ってきたらトリガー入力されている</returns>
		bool IsTrigger(EnButton button) const
		{
			return m_trigger[button] != 0;
		}
		
		/// <summary>
		/// ボタンが押されているか判定
		/// </summary>
		/// <param name="button">判定したいボタン</param>
		/// <returns>trueが返ってきたら入力されている。</returns>
		bool IsPress(EnButton button) const
		{
			return m_press[button] != 0;
		}

		/// <summary>
		/// 何かのボタンが押されているか判定。
		/// </summary>
		/// <returns>trueが返ってきたら押されている。</returns>
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
		/// 左スティックのx軸の入力量を取得
		/// </summary>
		/// <returns>-1.0～1.0の正規化された値を返す。</returns>
		float GetLStickXF() const
		{
			return m_lStickX;
		}
		
		/// <summary>
		/// 左スティックのY軸の入力量を取得。
		/// </summary>
		/// <returns>-1.0～1.0の正規化された値を返す。</returns>
		float GetLStickYF() const
		{
			return m_lStickY;
		}
		

		/// <summary>
		/// 右スティックのX軸の入力量を取得。
		/// </summary>
		/// <returns>-1.0～1.0の正規化された値を返す。</returns>
		float GetRStickXF() const
		{
			return m_rStickX;
		}
		
		/// <summary>
		/// 右スティックのY軸の入力量を取得
		/// </summary>
		/// <returns>-1.0～1.0の正規化された値を返す。</returns>
		float GetRStickYF() const
		{
			return m_rStickY;
		}
		
		
		/// <summary>
		/// フレームの開始時に呼び出す必要がある関数
		/// </summary>
		static void BeginFrame();
		/// <summary>
		/// XInputから引っ張ってきた生データを取得。
		/// </summary>
		/// <returns></returns>
		const XINPUT_STATE& GetXInputState()
		{
			return m_state.state;
		}
		/// <summary>
		/// バイブレーションを設定。
		/// </summary>
		/// <param name="padNo">パッド番号</param>
		/// <param name="durationSec">バイブレーションの継続時間(0.0～1.0)</param>
		/// <param name="normalizedPower">正規化されたパワー(0.0～1.0)</param>
		void SetVibration( float durationSec, float normalizedPower )
		{
			auto vibration = NewGO< GamePadVibration>(0);
			vibration->Init(m_padNo, durationSec, normalizedPower);
		}
	private:
		void UpdateCore(XINPUT_STATE xInputState);
	public:
		/// <summary>
		/// 最大パッド数
		/// </summary>
		enum { MAX_PAD = 4 };

		/// <summary>
		/// XInputパッドのステート。
		/// </summary>
		enum class EnXInputPadState {
			Undef,		//不明。
			Connect,	//接続されている。
			Disconnect,	//接続されていない。
		};
	private:
		PAD_STATE m_state;			// パッドステート。
		int m_padNo = 0;			// パッド番号。
		int m_trigger[enButtonNum];	// トリガー入力のフラグ。
		int m_press[enButtonNum];	// press入力のフラグ。
		float m_lStickX = 0.0f;		// 左スティックのX軸の入力量。
		float m_lStickY = 0.0f;		// 左スティックのY軸の入力量。
		float m_rStickX = 0.0f;		// 右スティックのX軸の入力量。
		float m_rStickY = 0.0f;		// 右スティックのY軸の入力量。

		static EnXInputPadState m_padStates[MAX_PAD];	// パッドの状態。
	};

	extern std::array<GamePad*, GamePad::CONNECT_PAD_MAX> g_pad;
}