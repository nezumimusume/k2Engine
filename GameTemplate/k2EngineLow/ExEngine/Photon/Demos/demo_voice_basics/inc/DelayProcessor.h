#include "PhotonVoice-cpp/inc/VoiceFramed.h"
#include "PhotonVoice-cpp/inc/Buffer.h"

template <typename T>
class DelayProcessor : public ExitGames::Voice::IProcessor<T>
{
public:
	DelayProcessor(int delaySamples, float factor)
		: mFactor(factor)
		, mPrevBuf(delaySamples*2)
		, mPrevBufPosWrite(0)
		, mPrevBufPosRead(delaySamples)
	{
	}

	~DelayProcessor(void)
	{
	}

	ExitGames::Voice::Buffer<T> process(ExitGames::Voice::Buffer<T> buf)
	{
		T* b = buf.getArray();
		for(int i=0; i<buf.getSize(); i++)
			b[i] += (T)(b[i] + mFactor * mPrevBuf.getArray()[mPrevBufPosRead++ % mPrevBuf.getSize()]);

		if(buf.getSize() > mPrevBuf.getSize() - mPrevBufPosWrite)
		{
			memcpy(mPrevBuf.getArray() + mPrevBufPosWrite, buf.getArray(), (mPrevBuf.getSize() - mPrevBufPosWrite)*sizeof(T));
			int newPos = buf.getSize() - (mPrevBuf.getSize() - mPrevBufPosWrite);
			memcpy(mPrevBuf.getArray(), buf.getArray() + mPrevBuf.getSize() - mPrevBufPosWrite, newPos*sizeof(T));
			mPrevBufPosWrite = newPos;
		}
		else
		{
			memcpy(mPrevBuf.getArray() + mPrevBufPosWrite, buf.getArray(), buf.getSize()*sizeof(T));
			mPrevBufPosWrite += buf.getSize();
		}
		return buf;
	}
private:
	float mFactor;
	ExitGames::Voice::Buffer<T> mPrevBuf;
	int mPrevBufPosWrite;
	int mPrevBufPosRead;
};
