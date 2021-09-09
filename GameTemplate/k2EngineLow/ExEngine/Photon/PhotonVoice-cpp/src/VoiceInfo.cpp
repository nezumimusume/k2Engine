/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/RemoteVoiceInfo.h"
#include "PhotonVoice-cpp/inc/VoiceAudio.h"

/** @file PhotonVoice-cpp/inc/VoiceInfo.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;

		VoiceInfo::VoiceInfo(void)
			: mCodec(0)
			, mSamplingRate(0)
			, mChannels(0)
			, mFrameDurationUs(0)
			, mBitrate(0)
			, mWidth(0)
			, mHeight(0)
		{
		}
		
		/// <summary>
		/// Create stream info for an Opus audio stream basing on audio source.
		/// </summary>
		/// <param name="samplingRate">Audio sampling rate. This must be one of 8000, 12000, 16000, 24000, or 48000.</param>
		/// <param name="channels">Number of channels.</param>
		/// <param name="frameDurationUs">Uncompressed frame (audio packet) size in microseconds. This must be one of 2500, 5000, 10000, 20000, 40000 or 60000.</param>
		/// <param name="bitrate">Stream bitrate (in bits/second).</param>
		/// <param name="userdata">Optional user data. Should be serializable by Photon.</param>
		/// <returns>VoiceInfo instance.</returns>
		VoiceInfo VoiceInfo::createAudioOpus(int samplingRate, int channels, int frameDurationUs, int bitrate, const Object& userdata)
		{
			return VoiceInfo()
				.setCodec(Codec::AUDIO_OPUS)
				.setSamplingRate(samplingRate)
				.setChannels(channels)
				.setFrameDurationUs(frameDurationUs)
				.setBitrate(bitrate)
				.setUserData(userdata)
				;
		}
#if EG_PHOTON_VOICE_VIDEO_ENABLE
		/// <summary>
		/// Helper for VP8 stream info creation.
		/// </summary>
		/// <param name="bitrate">Stream bitrate.</param>
		/// <param name="width">Streamed video width. If 0, width and height of video source used (no rescaling).</param>
		/// <param name="heigth">Streamed video height. If -1, aspect ratio preserved during rescaling.</param>
		/// <param name="userdata">Optional user data. Should be serializable by Photon.</param>        
		/// <returns>VoiceInfo instance.</returns>
		VoiceInfo VoiceInfo::createVideoVP8(int bitrate, int width, int heigth, const Object& userdata)
		{
			return VoiceInfo()
				.setCodec(Codec::VIDEO_VP8)
				.setBitrate(bitrate)
				.setWidth(width)
				.setHeight(heigth)
				.setUserData(userdata)
				;
		}
#endif

		JString& VoiceInfo::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"c=" + mCodec
				+ L" f=" + mSamplingRate
				+ L" ch=" + mChannels
				+ L" d=" + mFrameDurationUs
				+ L" s=" + getFrameSize()
				+ L" b=" + mBitrate
				+ L" w=" + mWidth
				+ L" h=" + mHeight
				+ L" ud=" + mUserData.toString(withTypes)
				+ L"}";
		}

		int VoiceInfo::getCodec(void) const
		{
			return mCodec;
		}

		VoiceInfo& VoiceInfo::setCodec(int codec)
		{
			mCodec = codec;
			return *this;
		}

		/// <summary>Audio sampling rate (frequency, in Hz).</summary>
		int VoiceInfo::getSamplingRate(void) const
		{
			return mSamplingRate;
		}

		VoiceInfo& VoiceInfo::setSamplingRate(int samplingRate)
		{
			mSamplingRate = samplingRate;
			return *this;
		}

		/// <summary>Number of channels.</summary>
		int VoiceInfo::getChannels(void) const
		{
			return mChannels;
		}

		VoiceInfo& VoiceInfo::setChannels(int channels)
		{
			mChannels = channels;
			return *this;
		}

		/// <summary>Uncompressed frame (audio packet) size in microseconds.</summary>
		int VoiceInfo::getFrameDurationUs(void) const
		{
			return mFrameDurationUs;
		}

		VoiceInfo& VoiceInfo::setFrameDurationUs(int frameDurationUs)
		{
			mFrameDurationUs = frameDurationUs;
			return *this;
		}

		/// <summary>Target bitrate (in bits/second).</summary>
		int VoiceInfo::getBitrate(void) const
		{
			return mBitrate;
		}

		VoiceInfo& VoiceInfo::setBitrate(int bitrate)
		{
			mBitrate = bitrate;
			return *this;
		}

		/// <summary>Optional user data. Should be serializable by Photon.</summary>
		const Object& VoiceInfo::getUserData(void) const
		{
			return mUserData;
		}

		VoiceInfo& VoiceInfo::setUserData(const Object& userData)
		{
			mUserData = userData;
			return *this;
		}

		/// <summary>Uncompressed frame (data packet) size in samples.</summary>
		int VoiceInfo::getFrameDurationSamples(void) const
		{
			return static_cast<int>(mSamplingRate*static_cast<int64>(mFrameDurationUs)/1000000);
		}

		/// <summary>Uncompressed frame (data packet) array size.</summary>
		int VoiceInfo::getFrameSize(void) const
		{
			return getFrameDurationSamples()*mChannels;
		}

		/// <summary>Video width (optional).</summary>
		int VoiceInfo::getWidth(void) const
		{
			return mWidth;
		}

		VoiceInfo& VoiceInfo::setWidth(int width)
		{
			mWidth = width;
			return *this;
		}

		/// <summary>Video height (optional)</summary>
		int VoiceInfo::getHeight(void) const
		{
			return mHeight;
		}

		VoiceInfo& VoiceInfo::setHeight(int height)
		{
			mHeight = height;
			return *this;
		}

		RemoteVoiceInfo::RemoteVoiceInfo(int channelId, int playerId, nByte voiceId, VoiceInfo info)
			: mChannelId(channelId)
			, mPlayerId(playerId)
			, mVoiceId(voiceId)
			, mInfo(info)
		{
		}

		/// <summary>Remote voice info.</summary>
		const VoiceInfo& RemoteVoiceInfo::getInfo(void) const
		{
			return mInfo;
		}

		/// <summary>ID of channel used for transmission.</summary>
		int RemoteVoiceInfo::getChannelId(void) const
		{
			return mChannelId;
		}

		/// <summary>Player ID of voice owner.</summary>
		int RemoteVoiceInfo::getPlayerId(void) const
		{
			return mPlayerId;
		}

		/// <summary>Voice ID (unique in the room).</summary>
		nByte RemoteVoiceInfo::getVoiceId(void) const
		{
			return mVoiceId;
		}

		JString& RemoteVoiceInfo::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"channelId=" + mChannelId
				+ L" playerId=" + mPlayerId
				+ L" voiceId=" + mVoiceId
				+ L" info=" + mInfo.toString(withTypes)
				+ L"}";
		}
	}
}