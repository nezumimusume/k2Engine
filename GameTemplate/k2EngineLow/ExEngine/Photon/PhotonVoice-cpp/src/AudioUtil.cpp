/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/AudioUtil.h"

/** @file PhotonVoice-cpp/inc/AudioUtil.h */

namespace ExitGames
{
	namespace Voice
	{
		/// <summary>Collection of Audio Utility functions and classes.</summary>
		namespace AudioUtil
		{
			using namespace Common;
			using namespace Common::MemoryManagement;

			/// <summary>Resample audio data so that the complete src buffer fits into dstCount samples in the dst buffer.</summary>
			/// This implements a primitive nearest-neighbor resampling algorithm for an arbitrary number of channels.
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="dstCount">Target size of destination buffer (in samples per channel).</param>
			/// <param name="channels">Number of channels in the signal (1=mono, 2=stereo). Must be > 0.</param>
			template<typename T>
			void resample(const Buffer<T>& srcBuf, Buffer<T>& dstBuf, int dstCount, int channels)
			{
				const T* src = srcBuf.getArray();
				T* dst = dstBuf.getArray();
				int srcCount = srcBuf.getSize();
				if(channels == 1)
					for(int i=0; i<dstCount; i++)
						dst[i] = src[i*srcCount/dstCount];
				else if(channels == 2)
				{
					for(int i=0; i<dstCount/2; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*2;
						int srcCh0I = srcI*2;
						dst[dstCh0I++] = src[srcCh0I++];
						dst[dstCh0I] = src[srcCh0I];
					}
				}
				else
				{
					for(int i=0; i<dstCount/channels; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*channels;
						int srcCh0I = srcI*channels;
						for(int ch = 0; ch < channels; ch++)
							dst[dstCh0I++] = src[srcCh0I++];
					}
				}
			}

			/// <summary>Resample audio data so that the complete src buffer fits into dstCount samples in the dst buffer,
			/// and convert short to float samples along the way.</summary>
			/// This implements a primitive nearest-neighbor resampling algorithm for an arbitrary number of channels.
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="dstCount">Target size of destination buffer (in samples per channel).</param>
			/// <param name="channels">Number of channels in the signal (1=mono, 2=stereo). Must be > 0.</param>
			void resampleAndConvert(const Buffer<short>& srcBuf, Buffer<float>& dstBuf, int dstCount, int channels)
			{
				const short* src = srcBuf.getArray();
				float* dst = dstBuf.getArray();
				int srcCount = srcBuf.getSize();
				if(channels == 1)
					for(int i=0; i<dstCount; i++)
						dst[i] = src[i*srcCount/dstCount]/static_cast<float>(SHRT_MAX);
				else if(channels == 2)
				{
					for(int i=0; i<dstCount/2; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*2;
						int srcCh0I = srcI*2;
						dst[dstCh0I++] = src[srcCh0I++]/static_cast<float>(SHRT_MAX);
						dst[dstCh0I] = src[srcCh0I]/static_cast<float>(SHRT_MAX);
					}
				}
				else
				{
					for(int i=0; i<dstCount/channels; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*channels;
						int srcCh0I = srcI*channels;
						for(int ch=0; ch<channels; ch++)
							dst[dstCh0I++] = src[srcCh0I++]/static_cast<float>(SHRT_MAX);
					}
				}
			}

			/// <summary>Resample audio data so that the complete src buffer fits into dstCount samples in the dst buffer,
			/// and convert float to short samples along the way.</summary>
			/// This implements a primitive nearest-neighbor resampling algorithm for an arbitrary number of channels.
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="dstCount">Target size of destination buffer (in samples per channel).</param>
			/// <param name="channels">Number of channels in the signal (1=mono, 2=stereo). Must be > 0.</param>
			void resampleAndConvert(const Buffer<float>& srcBuf, Buffer<short>& dstBuf, int dstCount, int channels)
			{
				const float* src = srcBuf.getArray();
				short* dst = dstBuf.getArray();
				int srcCount = srcBuf.getSize();
				if(channels == 1)
					for(int i=0; i<dstCount; i++)
						dst[i] = static_cast<short>(src[i*srcCount/dstCount]*static_cast<float>(SHRT_MAX));
				else if(channels == 2)
				{
					for(int i=0; i<dstCount/2; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*2;
						int srcCh0I = srcI*2;
						dst[dstCh0I++] = static_cast<short>(src[srcCh0I++]*static_cast<float>(SHRT_MAX));
						dst[dstCh0I] = static_cast<short>(src[srcCh0I]*static_cast<float>(SHRT_MAX));
					}
				}
				else
				{
					for(int i=0; i<dstCount/channels; i++)
					{
						int srcI = i*srcCount/dstCount;
						int dstCh0I = i*channels;
						int srcCh0I = srcI*channels;
						for(int ch=0; ch<channels; ch++)
							dst[dstCh0I++] = static_cast<short>(src[srcCh0I++]*static_cast<float>(SHRT_MAX));
					}
				}
			}

			/// <summary>Convert audio buffer from float to short samples.</summary>
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="dstCount">Size of destination buffer (in total samples), source buffer must be of same length or longer.</param>
			void convert(const Buffer<float>& srcBuf, Buffer<short>& dstBuf, int dstCount)
			{
				const float* src = srcBuf.getArray();
				short* dst = dstBuf.getArray();
				for(int i=0; i<dstCount; i++)
					dst[i] = static_cast<short>(src[i]*static_cast<float>(SHRT_MAX));
			}

			/// <summary>Convert audio buffer from short to float samples.</summary>
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="dstCount">Size of destination buffer (in total samples), source buffer must be of same length or longer.</param>
			void convert(const Buffer<short>& srcBuf, Buffer<float>& dstBuf, int dstCount)
			{
				const short* src = srcBuf.getArray();
				float* dst = dstBuf.getArray();
				for(int i=0; i<dstCount; i++)
					dst[i] = src[i]/static_cast<float>(SHRT_MAX);
			}


			/// <summary>Convert audio buffer with arbitrary number of channels to stereo.</summary>
			/// For mono sources (srcChannels==1), the signal will be copied to both Left and Right stereo channels.
			/// For all others, the first two available channels will be used, any other channels will be discarded.
			/// <param name="srcBuf">Source buffer.</param>
			/// <param name="dstBuf">Destination buffer.</param>
			/// <param name="srcChannels">Number of (interleaved) channels in src.</param>
			template<typename T>
			void forceToStereo(const Buffer<T>& srcBuf, Buffer<T>& dstBuf, int srcChannels)
			{
				T* src = srcBuf.getArray();
				T* dst = dstBuf.getArray();
				for(int i=0, j=0; j<dst->Length-1; i+=srcChannels, j+=2)
				{
					dst[j] = src[i];
					dst[j + 1] = srcChannels > 1 ? src[i + 1] : src[i];
				}
			}

			template<typename T>
			Resampler<T>::Resampler(int dstSize, int channels)
				: mFrameResampled(dstSize)
				, mChannels(channels)
			{
			}

			template<typename T>
			Buffer<T> Resampler<T>::process(Buffer<T> buf)
			{
				resample(buf, mFrameResampled, mFrameResampled.getSize(), mChannels);
				return mFrameResampled;
			}

			/// <summary>Create new LevelMeterFloat instance.</summary>
			/// <param name="samplingRate">Sampling rate of the audio signal (in Hz).</param>
			/// <param name="numChannels">Number of channels in the audio signal.</param>
			template<typename T>
			LevelMeter<T>::LevelMeter(int samplingRate, int numChannels)
				: mAmpSum(0)
				, mAmpPeak(0)
				, mPrevValues(samplingRate*numChannels/2) // 1/2 sec
				, mPrevValuesHead(0)
				, mAccumAvgPeakAmpSum(0)
				, mAccumAvgPeakAmpCount(0)
				, mCurrentPeakAmp(0)
			{
			}

			template<typename T>
			float LevelMeter<T>::getCurrentAvgAmp(void) const
			{
				return mAmpSum/mPrevValues.getSize();
			}

			template<typename T>
			float LevelMeter<T>::getCurrentPeakAmp(void) const
			{
				return mCurrentPeakAmp;
			}

			template<typename T>
			float LevelMeter<T>::getAccumAvgPeakAmp(void) const
			{
				return !mAccumAvgPeakAmpCount ? 0 : mAccumAvgPeakAmpSum/mAccumAvgPeakAmpCount;
			}

			template<typename T>
			void LevelMeter<T>::resetAccumAvgPeakAmp(void)
			{
				mAccumAvgPeakAmpSum = 0;
				mAccumAvgPeakAmpCount = 0;
				mAmpPeak = 0;
			}

			template<typename T>
			Buffer<T> LevelMeter<T>::process(Buffer<T> buf)
			{
				T* v = buf.getArray();
				for(int i=0; i<buf.getSize(); i++, v++)
				{
					T a = *v;
					if(a < 0)
						a = -a;
					mAmpSum = mAmpSum + a - mPrevValues.getArray()[mPrevValuesHead];
					mPrevValues.getArray()[mPrevValuesHead] = a;

					if(mAmpPeak < a)
						mAmpPeak = a;
					if(!mPrevValuesHead)
					{
						mCurrentPeakAmp = mAmpPeak;
						mAmpPeak = 0;
						mAccumAvgPeakAmpSum += mCurrentPeakAmp;
						mAccumAvgPeakAmpCount++;
					}
					mPrevValuesHead = (mPrevValuesHead + 1) % buf.getSize();
				}
				return buf;
			}

			template<typename T>
			bool VoiceDetectorCalibration<T>::isCalibrating(void) const
			{
				return mCalibrateCount > 0;
			}

			/// <summary>Create new VoiceDetectorCalibration instance.</summary>
			/// <param name="pVoiceDetector">Voice Detector to calibrate.</param>
			/// <param name="pLevelMeter">Level Meter to look at for calibration.</param>
			/// <param name="samplingRate">Sampling rate of the audio signal (in Hz).</param>
			/// <param name="channels">Number of channels in the audio signal.</param>
			template<typename T>
			VoiceDetectorCalibration<T>::VoiceDetectorCalibration(IVoiceDetector* pVoiceDetector, ILevelMeter* pLevelMeter, int samplingRate, int channels)
				: mpVoiceDetector(pVoiceDetector)
				, mpLevelMeter(pLevelMeter)
				, mValuesPerSec(samplingRate*channels)
				, mCalibrateCount(0)
			{
			}

			/// <summary>Start calibration.</summary>
			/// <param name="durationMs">Duration of the calibration procedure (in milliseconds).</param>
			/// This activates the Calibration process. 
			/// It will reset the given LevelMeter's AccumAvgPeakAmp (accumulated average peak amplitude),
			/// and when the duration has passed, use it for the VoiceDetector's detection threshold.
			template<typename T>
			void VoiceDetectorCalibration<T>::calibrate(int durationMs)
			{
				mCalibrateCount = mValuesPerSec*durationMs/1000;
				mpLevelMeter->resetAccumAvgPeakAmp();
			}

			template<typename T>
			Buffer<T> VoiceDetectorCalibration<T>::process(Buffer<T> buf)
			{
				if(mCalibrateCount)
				{
					mCalibrateCount -= buf.getSize();
					if(mCalibrateCount <= 0)
					{
						mCalibrateCount = 0;
						mpVoiceDetector->setThreshold(mpLevelMeter->getAccumAvgPeakAmp()*2);
					}
				}
				return buf;
			}

			template<typename T>
			Buffer<T> VoiceDetector<T>::process(Buffer<T> buffer)
			{
				if(mOn)
				{
					T* s = buffer.getArray();
					for(int i=0; i<buffer.getSize(); i++, s++)
					{
						if(*s > mThreshold)
						{
							setDetected(true);
							mAutoSilenceCounter = 0;
						}
						else
							mAutoSilenceCounter++;
					}
					if(mAutoSilenceCounter > mActivityDelayValuesCount)
						mDetected = false;

					return mDetected ? buffer : Buffer<T>::getEmpty();
				}
				else
					return buffer;
			}

			/// <summary>If true, voice detection enabled.</summary>
			template<typename T>
			bool VoiceDetector<T>::getOn(void) const
			{
				return mOn;
			}

			template<typename T>
			void VoiceDetector<T>::setOn(bool on)
			{
				mOn = on;
			}

			/// <summary>Voice detected as soon as signal level exceeds threshold.</summary>
			template<typename T>
			float VoiceDetector<T>::getThreshold(void) const
			{
				return mThreshold;
			}

			template<typename T>
			void VoiceDetector<T>::setThreshold(float value)
			{
				mThreshold = value;
			}

			template<typename T>
			bool VoiceDetector<T>::getDetected(void) const
			{
				return mDetected;
			}

			template<typename T>
			void VoiceDetector<T>::setDetected(bool value)
			{
				if(mDetected != value)
				{
					mDetected = value;
					mDetectedTime = GETTIMEMS();
					if(mDetected && mpOnDetected)
						mpOnDetected();
				}
			}
			/// <summary>Last time when switched to detected state.</summary>

			template<typename T>
			int VoiceDetector<T>::getDetectedTime(void) const
			{
				return mDetectedTime;
			}

			/// <summary>Called when switched to detected state.</summary>
			template<typename T>
			void VoiceDetector<T>::setOnDetected(void(*pCallback)(void))
			{
				mpOnDetected = pCallback;
			}

			/// <summary>Keep detected state during this time after signal level dropped below threshold.</summary>
			template<typename T>
			int VoiceDetector<T>::getActivityDelayMs(void) const
			{
				return mActivityDelayMs;
			}

			template<typename T>
			void VoiceDetector<T>::setActivityDelayMs(int value)
			{
				mActivityDelayMs = value;
				mActivityDelayValuesCount = value*mValuesCountPerSec/1000;
			}

			template<typename T>
			float defaultThreshold(void)
			{
				return 0;
			}

			template<>
			float defaultThreshold<float>(void)
			{
				return 0.01f;
			}

			template<>
			float defaultThreshold<short>(void)
			{
				return 0.01f*SHRT_MAX;
			}

			template<typename T>
			VoiceDetector<T>::VoiceDetector(int samplingRate, int numChannels)
				: mAutoSilenceCounter(0)
				, mValuesCountPerSec(samplingRate*numChannels)
				, mActivityDelayValuesCount(0)
				, mActivityDelayMs(0)
				, mOn(true)
				, mThreshold(defaultThreshold<T>())
				, mDetected(false)
				, mDetectedTime(0)
				, mpOnDetected(NULL)
			{
				setActivityDelayMs(500);
			}

			/// <summary>The LevelMeter in use.</summary>
			template<typename T>
			ILevelMeter* VoiceLevelDetectCalibrate<T>::getLevelMeter(void) const
			{
				return mpLevelMeter;
			}

			/// <summary>The VoiceDetector in use</summary>
			template<typename T>
			IVoiceDetector* VoiceLevelDetectCalibrate<T>::getDetector(void) const
			{
				return mpVoiceDetector;
			}

			/// <summary>Create new VoiceLevelDetectCalibrate instance</summary>
			/// <param name="samplingRate">Sampling rate of the audio signal (in Hz).</param>
			/// <param name="channels">Number of channels in the audio signal.</param>
			template<typename T>
			VoiceLevelDetectCalibrate<T>::VoiceLevelDetectCalibrate(int samplingRate, int channels)
				: mpLevelMeter(allocate<LevelMeter<T> >(samplingRate, channels))
				, mpVoiceDetector(allocate<VoiceDetector<T> >(samplingRate, channels))
				, mpCalibration(allocate<VoiceDetectorCalibration<T> >(mpVoiceDetector, mpLevelMeter, samplingRate, channels))
			{
			}

			template<typename T>
			VoiceLevelDetectCalibrate<T>::~VoiceLevelDetectCalibrate(void)
			{
				deallocate(mpLevelMeter);
				deallocate(mpVoiceDetector);
				deallocate(mpCalibration);
			}

			/// <summary>Start calibration</summary>
			/// <param name="durationMs">Duration of the calibration procedure (in milliseconds).</param>
			/// This activates the Calibration process. 
			/// It will reset the given LevelMeter's AccumAvgPeakAmp (accumulated average peak amplitude),
			/// and when the duration has passed, use it for the VoiceDetector's detection threshold.
			template<typename T>
			void VoiceLevelDetectCalibrate<T>::calibrate(int durationMs)
			{
				mpCalibration->calibrate(durationMs);
			}

			template<typename T>
			Buffer<T> VoiceLevelDetectCalibrate<T>::process(Buffer<T> buf)
			{
				return mpVoiceDetector->process(mpCalibration->process(mpLevelMeter->process(buf)));
			}

			template class Resampler<float>;
			template class Resampler<short>;
			template class VoiceDetectorCalibration<float>;
			template class VoiceDetectorCalibration<short>;
			template class VoiceLevelDetectCalibrate<float>;
			template class VoiceLevelDetectCalibrate<short>;
			template class LevelMeter<float>;
			template class LevelMeter<short>;
			template class VoiceDetector<float>;
			template class VoiceDetector<short>;
		}
	}
}