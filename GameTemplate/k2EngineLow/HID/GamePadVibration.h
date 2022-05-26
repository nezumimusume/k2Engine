

#pragma once

#include "gameObject/GameObjectManager.h"
#include "gameObject/IGameObject.h"

namespace nsK2EngineLow {	
	class GamePadVibration : public IGameObject
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="padNo">パッド番号</param>
		/// <param name="durationSec">バイブレーションを継続させる時間(単位:秒)</param>
		/// <param name="normalizedPower">バイブレーションの正規化された力。</param>
		void Init(int padNo, float durationSec, float normalizedPower);
		bool Start() override;
		void Update() override;
	private:
		float m_durationSec = 0;				// バイブレーションを継続する時間(単位:秒)。
		float m_timerSec = 0;					// タイマー(単位:秒)。
		int m_padNo = 0;						// パッド番号。
		XINPUT_VIBRATION m_xinputVibration;		// XINPUTに渡すバイブレーションの情報。
	};
}