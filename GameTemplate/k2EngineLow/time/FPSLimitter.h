#pragma once

#include "Stopwatch.h"
namespace nsK2EngineLow {

	/// <summary>
	/// FPSに制限をかけるクラス。
	/// </summary>
	class FPSLimitter {
	public:
		/// <summary>
		/// 1フレームの開始時に呼び出してください。
		/// </summary>
		void BeginFrame()
		{
			m_sw.Start();
		}
		/// <summary>
		/// 指定されたFPSを超えて動作している場合、ウェイトをかける。
		/// </summary>
		void Wait()
		{
			//スピンロックを行う。
			// 1フレームの最小時間を計算
			float frameDeltaTimeMin = 1000.0f / m_maxFPS;
			float restTime = 0;
			do {
				m_sw.Stop();
				restTime = frameDeltaTimeMin - (int)m_sw.GetElapsedMillisecond();
			} while (restTime > 1.0f);
		}
		/// <summary>
		/// 現在設定されている最大FPSを取得。
		/// </summary>
		/// <returns></returns>
		int GetMaxFPS() const
		{
			return m_maxFPS;
		}
		/// <summary>
		/// 最大FPSを設定。
		/// </summary>
		/// <param name="maxFps"></param>
		void SetMaxFPS(int maxFPS)
		{
			m_maxFPS = maxFPS;
		}
	private:
		Stopwatch m_sw;
		int m_maxFPS = MAX_FPS;
	};
}