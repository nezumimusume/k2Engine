/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/OpusCodec.h"
#include "PhotonVoice-cpp/inc/VoiceAudio.h"
#include "PhotonVoice-cpp/inc/VoiceClient.h"

/** @file PhotonVoice-cpp/inc/VoiceAudio.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		AudioDesc::AudioDesc(int samplingRate, int channels)
			: mSamplingRate(samplingRate)
			, mChannels(channels)
		{
		}

		int AudioDesc::getSamplingRate(void) const
		{
			return mSamplingRate;
		}

		int AudioDesc::getChannels(void) const
		{
			return mChannels;
		}

		JString AudioDesc::getError(void) const
		{
			return mError;
		}

		template<typename T>
		AudioUtil::IVoiceDetector* LocalVoiceAudio<T>::getVoiceDetector(void) const
		{
			return mpVoiceDetector;
		}

		template<typename T>
		AudioUtil::ILevelMeter* LocalVoiceAudio<T>::getLevelMeter(void) const
		{
			return mpLevelMeter;
		}

		/// <summary>Trigger voice detector calibration process.</summary>
		/// While calibrating, keep silence. Voice detector sets threshold basing on measured background noise level.
		/// <param name="durationMs">Duration of calibration in milliseconds.</param>
		template<typename T>
		void LocalVoiceAudio<T>::voiceDetectorCalibrate(int durationMs)
		{
			mpVoiceDetectorCalibration->calibrate(durationMs);
		}

		/// <summary>True if the VoiceDetector is currently calibrating.</summary>
		template<typename T>
		bool LocalVoiceAudio<T>::getIsVoiceDetectorCalibrating(void) const
		{
			return mpVoiceDetectorCalibration->isCalibrating();
		}

		template<typename T>
		LocalVoiceAudio<T>::LocalVoiceAudio(VoiceClient* pVoiceClient, SharedPointer<IEncoder> spEncoder, nByte id, const VoiceInfo& voiceInfo, const IAudioDesc& audioSourceDesc, int channelId)
			: LocalVoiceFramed<T>(pVoiceClient, spEncoder, id, voiceInfo, channelId, voiceInfo.getSamplingRate() ? static_cast<int>(voiceInfo.getFrameSize()*static_cast<int64>(audioSourceDesc.getSamplingRate())/voiceInfo.getSamplingRate()) : voiceInfo.getFrameSize())
			, mpLevelMeter(NULL)
			, mpVoiceDetector(NULL)
			, mpVoiceDetectorCalibration(NULL)
			, mChannels(voiceInfo.getChannels())
		{
			if(audioSourceDesc.getSamplingRate() != voiceInfo.getSamplingRate())
			{
				pVoiceClient->getTransport()->logWarning(JString(L"[PV] Local voice #") + id + L" audio source frequency " + audioSourceDesc.getSamplingRate() + L" and encoder sampling rate " + voiceInfo.getSamplingRate() + L" do not match. Resampling will occur before encoding.");
				this->addPostProcessor(allocate<AudioUtil::Resampler<T> >(this->mInfo.getFrameSize(), mChannels));
			}
			// these processors go after resampler
			mpLevelMeter = allocate<AudioUtil::LevelMeter<T> >(this->mInfo.getSamplingRate(), this->mInfo.getChannels());
			mpVoiceDetector = allocate<AudioUtil::VoiceDetector<T> >(this->mInfo.getSamplingRate(), this->mInfo.getChannels());
			mpVoiceDetectorCalibration = allocate<AudioUtil::VoiceDetectorCalibration<T> >(mpVoiceDetector, mpLevelMeter, this->mInfo.getSamplingRate(), mChannels);
			// level meter and calibration should be processed even if no signal detected
			this->addPostProcessor(mpLevelMeter);
			this->addPostProcessor(mpVoiceDetectorCalibration);
			this->addPostProcessor(mpVoiceDetector);
		}

		template<typename T>
		LocalVoiceAudio<T>::~LocalVoiceAudio()
		{
		}

		template<typename T>
		IEncoder* LocalVoiceAudio<T>::createDefaultEncoder(const VoiceInfo& info)
		{
			switch(info.getCodec())
			{
			case Codec::AUDIO_OPUS:
				return allocate<Opus::Encoder<T> >(info, this->getLogger());
				break;
			default:
				return allocate<UnsupportedCodecError>(L"LocalVoiceAudio", info.getCodec(), this->getLogger());
				break;
			}
		}

		LocalVoiceAudioDummy LocalVoiceAudioDummy::mDummy;

		LocalVoiceAudioDummy::LocalVoiceAudioDummy(void)
			: mpVoiceDetector(allocate<AudioUtil::VoiceDetectorDummy>())
			, mpLevelMeter(allocate<AudioUtil::LevelMeterDummy>())
		{
		}

		LocalVoiceAudioDummy::~LocalVoiceAudioDummy(void)
		{
			deallocate(mpVoiceDetector);
			deallocate(mpLevelMeter);
		}

		AudioUtil::IVoiceDetector* LocalVoiceAudioDummy::getVoiceDetector(void) const 
		{ 
			return mpVoiceDetector; 
		}

		AudioUtil::ILevelMeter* LocalVoiceAudioDummy::getLevelMeter(void) const 
		{ 
			return mpLevelMeter; 
		}

		bool LocalVoiceAudioDummy::getIsVoiceDetectorCalibrating(void) const 
		{ 
			return false; 
		}

		void LocalVoiceAudioDummy::voiceDetectorCalibrate(int /*durationMs*/) 
		{
		}

		LocalVoiceAudioDummy& LocalVoiceAudioDummy::getDummy(void)
		{
			return mDummy;
		}

		template class LocalVoiceAudio<float>;
		template class LocalVoiceAudio<short>;
	}
}