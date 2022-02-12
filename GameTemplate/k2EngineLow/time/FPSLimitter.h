#pragma once

#include "Stopwatch.h"
namespace nsK2EngineLow {

	/// <summary>
	/// FPS�ɐ�����������N���X�B
	/// </summary>
	class FPSLimitter {
	public:
		/// <summary>
		/// 1�t���[���̊J�n���ɌĂяo���Ă��������B
		/// </summary>
		void BeginFrame()
		{
			m_sw.Start();
		}
		/// <summary>
		/// �w�肳�ꂽFPS�𒴂��ē��삵�Ă���ꍇ�A�E�F�C�g��������B
		/// </summary>
		void Wait()
		{
			//�X�s�����b�N���s���B
			// 1�t���[���̍ŏ����Ԃ��v�Z
			float frameDeltaTimeMin = 1000.0f / m_maxFPS;
			float restTime = 0;
			do {
				m_sw.Stop();
				restTime = frameDeltaTimeMin - (int)m_sw.GetElapsedMillisecond();
			} while (restTime > 1.0f);
		}
		/// <summary>
		/// ���ݐݒ肳��Ă���ő�FPS���擾�B
		/// </summary>
		/// <returns></returns>
		int GetMaxFPS() const
		{
			return m_maxFPS;
		}
		/// <summary>
		/// �ő�FPS��ݒ�B
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