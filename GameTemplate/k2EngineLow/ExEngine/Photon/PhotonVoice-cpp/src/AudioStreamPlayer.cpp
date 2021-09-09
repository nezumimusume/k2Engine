/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/AudioStreamPlayer.h"

/** @file PhotonVoice-cpp/inc/AudioStreamPlayer.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;

		// fast forward if we are more than this value before the desired position (stream pos - playDelaySamples)
		static const int MAX_PLAY_LAG_MS = 100;

		template<typename T>
		AudioStreamPlayer<T>::AudioStreamPlayer(const ILogger& logger, ISyncAudioOut<T>& audioOut, const JString& logPrefix, bool debugInfo)
			: mMaxPlayLagSamples(0)
			, mPlayDelaySamples(0)
			, mFrameSize(0)
			, mFrameSamples(0)
			, mStreamSamplePos(0)
			, mStreamSamplePosAvg(0)
			, mCurrentBufferLag(0)
			, mAudioOut(audioOut)
			, mLogger(logger)
			, mAudioOutStarted(false)
			, mLogPrefix(logPrefix)
			, mDebugInfo(debugInfo)
		{
		}

		template<typename T>
		int AudioStreamPlayer<T>::getLag(void) const
		{
			return mCurrentBufferLag;
		}

		template<typename T>
		bool AudioStreamPlayer<T>::isPlaying(void) const
		{
			return mAudioOut.isPlaying();
		}

		template<typename T>
		void AudioStreamPlayer<T>::start(int frequency, int channels, int frameSamples, int playDelayMs)
		{
			mFrameSamples = frameSamples;
			mFrameSize = frameSamples*channels;

			// add 1 frame samples to make sure that we have something to play when delay set to 0
			mMaxPlayLagSamples = MAX_PLAY_LAG_MS*frequency/1000 + mFrameSamples;
			mPlayDelaySamples = playDelayMs*frequency/1000 + mFrameSamples;

			// init with target value
			mCurrentBufferLag = mPlayDelaySamples;
			mStreamSamplePosAvg = mPlayDelaySamples;

			mStreamSamplePos = 0;
			mAudioOut.start(frequency, channels, mFrameSamples, MAX_PLAY_LAG_MS + playDelayMs);
			mAudioOutStarted = true;
		}

		// should be called in Update thread
		template<typename T>
		void AudioStreamPlayer<T>::service(void)
		{
			if(mAudioOutStarted)
			{
				mAudioOut.service();

				int playPos = mAudioOut.getPlaySamplePos(); // cache calculated value
				// average jittering value
				mCurrentBufferLag = (mCurrentBufferLag*39 + (mStreamSamplePos - playPos))/40;

				// calc jitter-free stream position based on clock-driven player position and average lag
				mStreamSamplePosAvg = playPos + mCurrentBufferLag;
				if(mStreamSamplePosAvg > mStreamSamplePos)
					mStreamSamplePosAvg = mStreamSamplePos;

				// start with given delay or when stream position is ok after overrun pause
				if(playPos < mStreamSamplePos - mPlayDelaySamples && !mAudioOut.isPlaying())
						mAudioOut.unPause();

				if(playPos > mStreamSamplePos - mFrameSamples)
				{
					if(mAudioOut.isPlaying())
					{
						if(mDebugInfo)
							mLogger.logWarning(mLogPrefix + L" player overrun: " + playPos + L"/" + mStreamSamplePos + L"(" + mStreamSamplePosAvg + L") = " + (mStreamSamplePos - playPos));

						// when nothing to play:
						// pause player  (useful in case if stream is stopped for good) ...
						mAudioOut.pause();

						// ... and rewind to proper position 
						playPos = mStreamSamplePos;
						mAudioOut.setPlaySamplePos(playPos);
						mCurrentBufferLag = mPlayDelaySamples;
					}
				}
				if(mAudioOut.isPlaying())
				{
					int lowerBound = mStreamSamplePos - mPlayDelaySamples - mMaxPlayLagSamples;
					if(playPos < lowerBound)
					{
						if(mDebugInfo)
							mLogger.logWarning(mLogPrefix + L" player underrun: " + playPos + L"/" + mStreamSamplePos + L"(" + mStreamSamplePosAvg + L") = " + (mStreamSamplePos - playPos));

						// if lag exceeds max allowable, fast forward to proper position                    
						playPos = mStreamSamplePos - mPlayDelaySamples;
						mAudioOut.setPlaySamplePos(playPos);
						mCurrentBufferLag = mPlayDelaySamples;
					}
				}

			}
		}

		// may be called on any thread
		template<typename T>
		void AudioStreamPlayer<T>::push(Buffer<T> frame)
		{
			if(!mAudioOutStarted)
				return;

			if(!frame.getSize())
				return;
			if(frame.getSize() != mFrameSize)
			{
				mLogger.logError(mLogPrefix + L" Audio frames are not of  size: " + frame.getSize() + L" != " + mFrameSize);
				return;
			}

			// Store last packet

			if(mAudioOutStarted)
			{
				mAudioOut.push(frame);
				mStreamSamplePos += mFrameSamples;
			}
		}

		template<typename T>
		void AudioStreamPlayer<T>::stop(void)
		{
			mAudioOut.stop();
			mAudioOutStarted = false;
		}

		template class AudioStreamPlayer<float>;
		template class AudioStreamPlayer<short>;
	}
}