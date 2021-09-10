#include "PhotonVoice-cpp/inc/AudioUtil.h"
#include "PhotonVoice-cpp/inc/VoiceAudio.h"
#include "PhotonVoice-cpp/inc/Buffer.h"
#include "../portaudio/include/portaudio.h"

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (128)
#define INPUT_CHANNELS 1
#define OUTPUT_CHANNELS 1

using namespace ExitGames::Common;
using namespace ExitGames::Common::Helpers;
using namespace ExitGames::Common::MemoryManagement;
using namespace ExitGames::Voice;

template<typename T>
static void* memAdd(void* dst, const void* src, size_t cnt);
static void copyAudio(float* dst, int dstSamples, int dstChannels, const float* src, int srcSamples, int srcChannels);



class IAudioIOProcessor : virtual public ToString
{
public:
	// returns true if output buffer has been written
	virtual bool audioProcessing(const void* input, void* output, unsigned long sampleCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData, bool outInited) = 0;
	virtual ~IAudioIOProcessor(void) {}
};

static Lockable<JVector<IAudioIOProcessor*> > gProcessors;



static void addProcessor(IAudioIOProcessor* p);
static void removeProcessor(IAudioIOProcessor* p);



// frame (frame sample rate, frame channels) -> push() -> ring in (out sampling rate, out channels) -> audioProcessing() -> ring out (out sampling rate, out channels)
class AudioOut : public ISyncAudioOut<float>, public IAudioIOProcessor
{
public:
	using ToString::toString;

	AudioOut(void);
	virtual ~AudioOut(void);

	virtual bool isPlaying(void) const;
	virtual void start(int frequency, int channels, int frameSamples, int playDelayMs);
	virtual void stop(void);
	virtual int getLag(void);
	virtual int getPlaySamplePos(void) const;
	virtual void setPlaySamplePos(int value0);
	virtual void pause(void);
	virtual void unPause(void);
	// may be called on any thread
	virtual void push(Buffer<float> frame);
	virtual bool audioProcessing(const void* pInput, void* pOutput, unsigned long outSamples, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData, bool outInited);
	virtual void service(void);
	virtual int getLag(void) const;
	virtual JString& toString(JString& retStr, bool withTypes=false) const;
private:
	int mRingPosIn;		// position in float[]
	int mRingPosOut;	// position in float[]
	int mOutLoopCount;
	int mFrameSampleRate;
	int mFrameChannels;
	Buffer<float> mRingBuf;
	int mRingBufSamples;
	mutable std::mutex mLock;

	bool mStarted;
	bool mPlaying;
};



class MicPusher : public IAudioPusher<float>, public IAudioIOProcessor
{
public:
	using ToString::toString;

	MicPusher(void);
	virtual ~MicPusher(void);

	// Inherited via IAudioPusher
	virtual void setCallback(void* pLocalVoice, void(*pCallback)(void*, const Buffer<float>&));

	virtual int getSamplingRate(void) const;
	virtual int getChannels(void) const;
	virtual JString getError(void) const;

	// Inherited via IAudioIOProcessor
	virtual bool audioProcessing(const void* pInput, void* pOutput, unsigned long frameCount, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData, bool outInited);
	virtual JString& toString(JString& retStr, bool withTypes=false) const;
private:
	void(*mpCallback)(void*, const Buffer<float>&);
	void* mpLocalVoice;
};



// Audio input/output.
static int audioProcessing(const void* pInput, void* pOutput, unsigned long outSamples, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData);
static void finitPortaudio(PaError reasonErr);

static PaStream* gStream = NULL;



template<typename T>
static void* memAdd(void* dst, const void* src, size_t cnt)
{
	T* dstT = static_cast<T*>(dst);
	const T* srcT = static_cast<const T*>(src);
	cnt /= sizeof(T);
	while(cnt--)
		*dstT++ += *srcT++;
	return dst;
}

static void copyAudio(float* dst, int dstSamples, int dstChannels, const float* src, int srcSamples, int srcChannels)
{
	if(dstSamples == srcSamples)
	{
		if(dstChannels == srcChannels)
		{
			int cnt = dstSamples * srcChannels;
			while(cnt--)
				*dst++ = *src++;
		}
		else if(dstChannels == 2 && srcChannels == 1)
		{
			int cnt = dstSamples;
			while(cnt--)
			{
				*dst++ = *src;
				*dst++ = *src++;
			}
		}
		else if(dstChannels == 1 && srcChannels == 2)
		{
			int cnt = dstSamples;
			while(cnt--)
				*dst++ = (*src++ + *src++) / 2;
		}
	}
	else
	{
		if(dstChannels == srcChannels)
		{
			if(dstChannels == 1)
				for(int i=0; i<dstSamples; ++i)
					dst[i] = src[i * srcSamples / dstSamples];
			else if(dstChannels == 2)
			{
				for(int i=0; i<dstSamples; ++i)
				{
					int srcCh = i * srcSamples / dstSamples * 2;
					*dst++ = src[srcCh];
					*dst++ = src[srcCh + 1];
				}
			}
		}
		else if(dstChannels == 2 && srcChannels == 1)
		{
			for(int i=0; i<dstSamples; ++i)
			{
				float v = src[i * srcSamples / dstSamples];
				*dst++ = v;
				*dst++ = v;
			}
		}
		else if(dstChannels == 1 && srcChannels == 2)
		{
			for(int i=0; i<dstSamples; ++i)
			{
				int srcCh = i * srcSamples / dstSamples * 2;
				*dst++ = (src[srcCh] + src[srcCh + 1]) / 2;
			}
		}
	}
}



static void addProcessor(IAudioIOProcessor* p)
{
	std::lock_guard<std::mutex> lock(gProcessors);
	gProcessors.addElement(p);
}

static void removeProcessor(IAudioIOProcessor* p)
{
	std::lock_guard<std::mutex> lock(gProcessors);
	while(gProcessors.removeElement(p));
}



AudioOut::AudioOut(void)
	: mRingPosIn(0)
	, mRingPosOut(0)
	, mOutLoopCount(0)
	, mStarted(0)
	, mPlaying(0)
	, mFrameSampleRate(0)
	, mFrameChannels(0)
	, mRingBufSamples(0)
{
	addProcessor(this);
}

AudioOut::~AudioOut(void)
{
	removeProcessor(this);
}

bool AudioOut::isPlaying(void) const
{
	return mPlaying;
}

void AudioOut::start(int frequency, int channels, int frameSamples, int playDelayMs)
{
	mFrameSampleRate = frequency;
	mFrameChannels = channels;
	mRingBufSamples = (playDelayMs * SAMPLE_RATE / 1000 + frameSamples * SAMPLE_RATE / mFrameSampleRate + SAMPLE_RATE);
	mRingBuf = Buffer<float>(OUTPUT_CHANNELS * mRingBufSamples); // max delay + frame +  1 sec. just in case
	mStarted = true;
	mRingPosIn = 0;
	mRingPosOut = 0;
	setPlaySamplePos(0);
	mPlaying = true;
}

void AudioOut::stop(void)
{
	mStarted = false;
}

int AudioOut::getLag(void)
{
	return 0;
}

int AudioOut::getPlaySamplePos(void) const
{
	std::lock_guard<std::mutex> lock(mLock);
	return mStarted ? (mOutLoopCount * mRingBufSamples + mRingPosOut / OUTPUT_CHANNELS) * (int64)mFrameSampleRate / SAMPLE_RATE : 0;
}

void AudioOut::setPlaySamplePos(int value0)
{
	std::lock_guard<std::mutex> lock(mLock);
	if(mStarted)
	{
		// scale to output sample rate
		int value = value0 * (int64)SAMPLE_RATE / mFrameSampleRate;
		// if a negative value is passed (possible when playback starts?), then the loop count is < 0 and the sample position is positive
		int pos = value % mRingBufSamples;
		if(pos < 0)
			pos += mRingBufSamples;

		mRingPosOut = pos * OUTPUT_CHANNELS;
		mOutLoopCount = value / mRingBufSamples;
	}
}

void AudioOut::pause(void)
{
	mPlaying = false;
	printf("||\n");
}

void AudioOut::unPause(void)
{
	mPlaying = true;
	printf(">>\n");
}

// may be called on any thread
void AudioOut::push(Buffer<float> frame)
{
	std::lock_guard<std::mutex> lock(mLock);
	if(frame.getSize() == 0)
		return;

	int inSamples = frame.getSize() / mFrameChannels * SAMPLE_RATE / mFrameSampleRate;

	int inRem = mRingBufSamples - mRingPosIn / OUTPUT_CHANNELS;
	if(inSamples <= inRem)
	{
		copyAudio(mRingBuf.getArray() + mRingPosIn, inSamples, OUTPUT_CHANNELS, frame.getArray(), frame.getSize() / mFrameChannels, mFrameChannels);
		mRingPosIn += inSamples * OUTPUT_CHANNELS;
	}
	else
	{
		int frameRem = (frame.getSize() / mFrameChannels) * inRem / (inSamples);
		copyAudio(mRingBuf.getArray() + mRingPosIn, inRem, OUTPUT_CHANNELS, frame.getArray(), frameRem, mFrameChannels);
		copyAudio(mRingBuf.getArray(), inSamples - inRem, OUTPUT_CHANNELS, frame.getArray() + frameRem, frame.getSize() / mFrameChannels - frameRem, mFrameChannels);
		mRingPosIn = (inSamples - inRem) * OUTPUT_CHANNELS;
	}
}

bool AudioOut::audioProcessing(const void* pInput, void* pOutput, unsigned long outSamples, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData, bool outInited)
{
	std::lock_guard<std::mutex> lock(mLock);
	if(!mPlaying)
		return false;

	int outRem = mRingBufSamples - mRingPosOut / OUTPUT_CHANNELS;
	if((int)outSamples <= outRem)
	{
		(outInited ? memAdd<float> : memcpy)(static_cast<float*>(pOutput), mRingBuf.getArray() + mRingPosOut, outSamples * OUTPUT_CHANNELS * sizeof(float));
		mRingPosOut += outSamples * OUTPUT_CHANNELS;
	}
	else
	{
		(outInited ? memAdd<float> : memcpy)(static_cast<float*>(pOutput), mRingBuf.getArray() + mRingPosOut, outRem * OUTPUT_CHANNELS * sizeof(float));
		(outInited ? memAdd<float> : memcpy)(static_cast<float*>(pOutput) + outRem * OUTPUT_CHANNELS, mRingBuf.getArray(), (outSamples - outRem) * OUTPUT_CHANNELS * sizeof(float));
		mRingPosOut = (outSamples - outRem) * OUTPUT_CHANNELS;
		mOutLoopCount++;
	}
	return true;
}

void AudioOut::service(void)
{
}

int AudioOut::getLag(void) const
{
	return 0;
}

JString& AudioOut::toString(JString& retStr, bool withTypes) const
{
	return retStr
		+= JString(L"{")
		+ L"ringPosIn: " + mRingPosIn
		+ L" ringPosOut: " + mRingPosOut
		+ L" outLoopCount: " + mOutLoopCount
		+ L" frameSampleRate: " + mFrameSampleRate
		+ L" frameChannels: " + mFrameChannels
		+ L" ringBuf: " + mRingBuf.toString(withTypes)
		+ L" ringBufSamples: " + mRingBufSamples
		+ L" started: " + (mStarted?L"true":L"false")
		+ L" playing: " + (mPlaying?L"true":L"false")
		+ L"}";
}



MicPusher::MicPusher(void)
	: mpCallback(NULL)
	, mpLocalVoice(NULL)
{
}

MicPusher::~MicPusher(void)
{
	removeProcessor(this);
}

// Inherited via IAudioPusher
void MicPusher::setCallback(void* pLocalVoice, void(*pCallback)(void*, const Buffer<float>&))
{
	mpLocalVoice = pLocalVoice;
	mpCallback = pCallback;
	addProcessor(this);
}

int MicPusher::getSamplingRate(void) const
{
	return SAMPLE_RATE;
}

int MicPusher::getChannels(void) const
{
	return INPUT_CHANNELS;
}

JString MicPusher::getError(void) const
{
	return JString();
}

// Inherited via IAudioIOProcessor
bool MicPusher::audioProcessing(const void* pInput, void* pOutput, unsigned long frameCount, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData, bool outInited)
{
	if(mpCallback)
	{
		Buffer<float> f(frameCount * getChannels());
		memcpy(f.getArray(), pInput, frameCount * getChannels() * sizeof(float));
		mpCallback(mpLocalVoice, f);
	}
	return false;
}

JString& MicPusher::toString(JString& retStr, bool withTypes) const
{
	return retStr += L"{" + Helpers::TypeName::get(this) + L"}";
}



static int audioProcessing(const void* pInput, void* pOutput, unsigned long outSamples, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData)
{
	std::lock_guard<std::mutex> lock(gProcessors);
	bool outInited = false;
	for(unsigned int i=0; i<gProcessors.getSize(); ++i)
		if(gProcessors[i]->audioProcessing(pInput, pOutput, outSamples, pTimeInfo, statusFlags, pUserData, outInited))
			outInited = true;
	if(!outInited)
		memset(pOutput, 0, outSamples * OUTPUT_CHANNELS * sizeof(float));
	return paContinue;
}

void initAudoIO(void)
{
	PaStreamParameters  inputParameters;
	PaStreamParameters  outputParameters;
	PaError             err = paNoError;

	printf("patest_record.c\n");
	fflush(stdout);

	err = Pa_Initialize();
	if(err != paNoError)
	{
		finitPortaudio(err);
		return;
	}

	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	if(inputParameters.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default input device.\n");
		finitPortaudio(paNoError);
		return;
	}
	inputParameters.channelCount = INPUT_CHANNELS; /* stereo input */
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if(outputParameters.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default output device.\n");
		finitPortaudio(paNoError);
		return;
	}
	outputParameters.channelCount = OUTPUT_CHANNELS; /* stereo output */
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&gStream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, /* we won't output out of range samples so don't bother clipping them */ audioProcessing, NULL);
	if(err != paNoError)
	{
		gStream = NULL;
		finitPortaudio(err);
		return;
	}

	err = Pa_StartStream(gStream);
	if(err != paNoError)
	{
		finitPortaudio(err);
		return;
	}
	printf("\n=== Now recording!! Please speak into the microphone. ===\n");
	fflush(stdout);
}

void finitAudoIO(void)
{
	finitPortaudio(paNoError);
}

static void finitPortaudio(PaError reasonErr)
{
	PaError err = paNoError;
	if(gStream)
	{
		err = Pa_CloseStream(gStream);
		if(err != paNoError)
		{
			fprintf(stderr, "Error %d while closing device.\n", err);
			fflush(stdout);
		}
	}

	Pa_Terminate();
	if(reasonErr != paNoError)
	{
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		err = 1; /* Always return 0 or 1, but no other return codes. */
	}
}

IAudioPusher<float>* audioIOCreateMic(void)
{
	return allocate<MicPusher>();
}

ISyncAudioOut<float>* audioIOCreateOut(void)
{
	return new AudioOut();
}