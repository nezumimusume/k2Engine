/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/AudioGen.h"

/** @file PhotonVoice-cpp/inc/AudioGen.h */

namespace ExitGames
{
	namespace Voice
	{
		namespace AudioUtil
		{
			using namespace Common;
			using namespace Common::Helpers;

			template<typename T>
			static float toneAmp(void)
			{
				return 0;
			}

			template<>
			float toneAmp<float>(void)
			{
				return 0.02f;
			}

			template<>
			float toneAmp<short>(void)
			{
				return 0.02f*SHRT_MAX;
			}

			/// <summary>Create a new ToneAudioPusher instance</summary>
			/// <param name="frequency">Frequency of the generated tone (in Hz).</param>
			/// <param name="bufSizeMs">Size of buffers to push (in milliseconds).</param>
			/// <param name="samplingRate">Sampling rate of the audio signal (in Hz).</param>
			/// <param name="channels">Number of channels in the audio signal.</param>
			template<typename T>
			ToneAudioPusher<T>::ToneAudioPusher(int frequency, int bufSizeMs, int samplingRate, int channels)
				: mSamplingRate(samplingRate)
				, mChannels(channels)
				, mAmp(toneAmp<T>())
				, mK(2*3.14159265358979323846*frequency/samplingRate)
				, mBufSizeSamples(bufSizeMs*samplingRate/1000)
				, mTimeMs(0)
				, mpLocalVoice(NULL)
				, mpCallback(NULL)
			{
			}

			template<typename T>
			ToneAudioPusher<T>::~ToneAudioPusher(void)
			{
			}

			/// <summary>Number of channels in the audio signal.</summary>
			template<typename T>
			int ToneAudioPusher<T>::getChannels(void) const
			{
				return mChannels;
			}

			/// <summary>Sampling rate of the audio signal (in Hz).</summary>
			template<typename T>
			int ToneAudioPusher<T>::getSamplingRate(void) const
			{
				return mSamplingRate;
			}

			/// <summary>If not an empty string, then the audio object is in invalid state.</summary>
			template<typename T>
			JString ToneAudioPusher<T>::getError(void) const
			{
				return JString();
			}

			/// <summary>Set the callback function used for pushing data</summary>
			/// <param name="pLocalVoice">Outgoing audio stream, for context</param>
			/// <param name="pCallback">Callback function to use</param>
			template<typename T>
			void ToneAudioPusher<T>::setCallback(void* pLocalVoice, void(*pCallback)(void*, const Buffer<T>&))
			{
				mpLocalVoice = pLocalVoice;
				mpCallback = pCallback;
				this->retain();
				Thread::create(workerStarter, this);
			}

			template<typename T>
			void ToneAudioPusher<T>::workerStarter(void* arg)
			{
				static_cast<ToneAudioPusher<T>*>(arg)->worker();
			}

			template<typename T>
			void ToneAudioPusher<T>::worker(void)
			{
				mTimeMs = GETTIMEMS();
				while(this->getRefCount() > 1)
				{
					Buffer<T> buf(mBufSizeSamples*mChannels);
					T* b = buf.getArray();

					int64 timeSamples = mTimeMs*mSamplingRate/1000;

					for(int i=0; i<mBufSizeSamples; i++)
					{
						double v = sin(timeSamples++*mK)*mAmp;
						for(int j=0; j<mChannels; j++)
							*b++ = static_cast<T>(v);
					}

					mTimeMs += mBufSizeSamples*1000/mSamplingRate;
					mpCallback(mpLocalVoice, buf);

					int dt = mTimeMs-GETTIMEMS();
					if(dt > 0)
						SLEEP(dt);
				}
				this->release();
			}

			template<typename T>
			JString& ToneAudioPusher<T>::toString(JString& retStr, bool /*withTypes*/) const
			{
				return retStr
					+= JString(L"{")
					+ L"samplingRate=" + mSamplingRate
					+ L" ch=" + mChannels
					+ L"}";
			}

			template class ToneAudioPusher<float>;
			template class ToneAudioPusher<short>;
		}
	}
}