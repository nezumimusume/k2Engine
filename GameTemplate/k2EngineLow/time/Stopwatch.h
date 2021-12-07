#pragma once

namespace nsK2EngineLow {

	/*!
	 *@brief	�X�g�b�v�E�H�b�`�N���X�B
	 *@details
	 * C#��Stopwatch�̂悤�Ɏg����B</br>
	 *@code
	   �T���v���R�[�h
	   Stopwatch sw;
	   sw.Start();		//�v���J�n�B
	   for(int i = 0; i < 100; i++ ){
	   }
	   sw.Stop();		//�v���I��
	   printf("�o�ߎ��� = %lf(�P�ʁF�b)\n", sw.GetElapsed());
	   printf("�o�ߎ��� = %lf(�P�ʁF�~��)\n", sw.GetElapsedMillisecond());
	 *@endcode
	 *
	 */
	class Stopwatch {
	public:
		

		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		Stopwatch()
		{
			freq = 0;
			end = 0;
			begin = 0;
			elapsed = 0.0;
			elapsedMill = 0.0;
			elapsedMicro = 0.0;
		}
		

		 /// <summary>
		 /// �f�X�g���N�^
		 /// </summary>
		~Stopwatch()
		{
		}
		
		/// <summary>
		/// �v���J�n
		/// </summary>
		void Start()
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&begin);
		}
		
		/// <summary>
		/// �v���I��
		/// </summary>
		void Stop()
		{
			::QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			::QueryPerformanceCounter((LARGE_INTEGER*)&end);
			elapsed = double(end - begin) / freq;
			elapsedMill = elapsed * 1000.0;
			elapsedMicro = elapsedMill * 1000.0;
		}
		

		/// <summary>
		/// �o�ߎ��Ԃ��擾(�P��:�b)
		/// </summary>
		/// <returns></returns>
		double GetElapsed() const
		{
			return elapsed;
		}
		/// <summary>
		/// �o�ߎ��Ԃ��擾(�P��:�~���b)
		/// </summary>
		/// <returns></returns>
		double GetElapsedMillisecond() const
		{
			return elapsedMill;
		}

		/// <summary>
		/// �o�ߎ��Ԃ��擾(�P��:�}�C�N���b)
		/// </summary>
		/// <returns></returns>
		double GetElapsedMicrosecond() const
		{
			return elapsedMicro;
		}
	private:
		LONGLONG freq;
		LONGLONG end;
		LONGLONG begin;
		double elapsed;			//�o�ߎ���(�P�ʁF�b)
		double elapsedMill;		//�o�ߎ���(�P�ʁF�~���b)
		double elapsedMicro;	//�o�ߎ���(�P�ʁF�}�C�N���b)

	};
}