/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceClient.h"

/** @file PhotonVoice-cpp/inc/Voice.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		int LocalVoice::getChannelId(void) const
		{
			return mChannelId;
		}

		/// <summary>If InterestGroup != 0, voice's data is sent only to clients listening to this group.</summary>
		nByte LocalVoice::getInterestGroup(void) const
		{
			return mInterestGroup;
		}

		void LocalVoice::setInterestGroup(nByte group)
		{
			mInterestGroup = group;
		}

		/// <summary>Returns Info structure assigned on local voice cration.</summary>
		const VoiceInfo& LocalVoice::getInfo(void) const
		{
			return mInfo;
		}

		nByte LocalVoice::getId(void) const
		{
			return mId;
		}

		nByte LocalVoice::getEvNumber(void) const
		{
			return mEvNumber;
		}

		bool LocalVoice::getTransmitEnabled(void) const
		{
			return mTransmitEnabled;
		}
		void LocalVoice::setTransmitEnabled(bool transmitEnabled)
		{
			mTransmitEnabled = transmitEnabled;
		}

		/// <summary>Returns true if stream broadcasts.</summary>
		bool LocalVoice::getIsCurrentlyTransmitting(void) const
		{
			return mIsCurrentlyTransmitting;
		}

		/// <summary>Sent frames counter.</summary>
		int LocalVoice::getFramesSent(void) const
		{
			return mFramesSent;
		}

		/// <summary>Sent frames bytes counter.</summary>
		int LocalVoice::getFramesSentBytes(void) const
		{
			return mFramesSentBytes;
		}

		/// <summary>Send data reliable.</summary>
		bool LocalVoice::getReliable(void) const
		{
			return mReliable;
		}

		void LocalVoice::setReliable(bool reliable)
		{
			mReliable = reliable;
		}

		/// <summary>Send data encrypted.</summary>
		bool LocalVoice::getEncrypt(void) const
		{
			return mEncrypt;
		}

		void LocalVoice::setEncrypt(bool encrypt)
		{
			mEncrypt = encrypt;
		}

		/// <summary>
		/// If true, outgoing stream routed back to client via server same way as for remote client's streams.
		/// Can be swithed any time. OnRemoteVoiceInfoAction and OnRemoteVoiceRemoveAction are triggered if required.
		/// This functionality availability depends on transport.
		/// </summary>
		bool LocalVoice::getDebugEchoMode(void) const
		{
			return mDebugEchoMode;
		}

		void LocalVoice::setDebugEchoMode(bool mode)
		{
			if(mDebugEchoMode != mode)
			{
				mDebugEchoMode = mode;
				if(mpVoiceClient && mpVoiceClient->getTransport())
					mpVoiceClient->getTransport()->setDebugEchoMode(*this);
			}
		}

		const ILogger& LocalVoice::getLogger(void) const
		{
			return *mpVoiceClient->getTransport();
		}

		JString LocalVoice::getName(void) const
		{
			return JString(L"Local v#") + mId + L" ch#" + mpVoiceClient->channelStr(mChannelId);
		}

		JString LocalVoice::getLogPrefix(void) const
		{
			return JString(L"[PV] ") + getName();
		}

		void LocalVoice::setupEncoder(void)
		{
			if(!mspEncoder)
			{
				getLogger().logInfo(getLogPrefix() + L": Creating default ecoder");
				mspEncoder = createDefaultEncoder(mInfo);
			}
			mspEncoder->setOutput(this, sendFrame);
		}

		IEncoder* LocalVoice::createDefaultEncoder(const VoiceInfo& info)
		{
			return allocate<UnsupportedCodecError>(L"LocalVoice", info.getCodec(), getLogger());
		}

		LocalVoice::LocalVoice() // for dummy voices
			: mId(0)
			, mChannelId(0)
			, mEvNumber(0)
			, mDebugEchoMode(false)
			, mpVoiceClient(NULL)
			, mspEncoder()
			, mInterestGroup(0)
			, mTransmitEnabled(false)
			, mIsCurrentlyTransmitting(false)
			, mFramesSent(0)
			, mFramesSentBytes(0)
			, mReliable(false)
			, mEncrypt(false)
			, mNoTransmitCnt(0)
		{
		}

		LocalVoice::LocalVoice(VoiceClient* pVoiceClient, SharedPointer<IEncoder> spEncoder, nByte id, const VoiceInfo& voiceInfo, int channelId)
			: mId(id)
			, mChannelId(channelId)
			, mEvNumber(0)
			, mInfo(voiceInfo)
			, mDebugEchoMode(false)
			, mpVoiceClient(pVoiceClient)
			, mspEncoder(spEncoder)
			, mInterestGroup(0)
			, mTransmitEnabled(true)
			, mIsCurrentlyTransmitting(false)
			, mFramesSent(0)
			, mFramesSentBytes(0)
			, mReliable(false)
			, mEncrypt(false)
			, mNoTransmitCnt(0)
		{
		}

		LocalVoice::~LocalVoice(void)
		{
		}

		void LocalVoice::resetNoTransmitCnt(void)
		{
			mNoTransmitCnt = 10; // timeout * service() calls frequency
		}

		void LocalVoice::service(void)
		{
			if(mpVoiceClient->getTransport()->isChannelJoined(mChannelId) && mTransmitEnabled)
			{
				Buffer<nByte> b;
				do
					if((b=mspEncoder->dequeueOutput()).getSize())
						sendFrame(b);
				while(b.getSize());
			}

			if(!mNoTransmitCnt)
				mIsCurrentlyTransmitting = false;
			else
			{
				mIsCurrentlyTransmitting = true;
				mNoTransmitCnt--;
			}
		}

		void LocalVoice::sendFrame(void* opaque, const Buffer<nByte>& buffer)
		{
			static_cast<LocalVoice*>(opaque)->sendFrame(buffer);
		}

		void LocalVoice::sendFrame(const Buffer<nByte>& compressed)
		{
			mFramesSent++;
			mFramesSentBytes += compressed.getSize();

			mpVoiceClient->getTransport()->sendFrame(compressed, mEvNumber, mId, mChannelId, *this);
			{
				Lockguard lock(mEventTimestamps);
				mEventTimestamps.put(mEvNumber, GETTIMEMS());
			}
			mEvNumber++;

			resetNoTransmitCnt();
		}

		/// <summary>Remove this voice from it's VoiceClient (using VoiceClient.RemoveLocalVoice</summary>.</summary>
		void LocalVoice::removeSelf(void)
		{
			if(mpVoiceClient) // dummy voice can try to remove self
				mpVoiceClient->removeLocalVoice(*this);
		}

		JString& LocalVoice::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"id=" + mId
				+ L" channelId=" + mChannelId
				+ L" evNumber=" + mEvNumber
				+ L" info=" + mInfo.toString(withTypes)
				+ L" debugEchoMode=" + (mDebugEchoMode?L"true":L"false")
				+ L" encoder=" + L"{" + Helpers::TypeName::get(mspEncoder) + L"}"
				+ L" interestGroup=" + mInterestGroup
				+ L" transmitEnabled=" + (mTransmitEnabled?L"true":L"false")
				+ L" isCurrentlyTransmitting=" + (mIsCurrentlyTransmitting?L"true":L"false")
				+ L" framesSent=" + mFramesSent
				+ L" framesSentBytes=" + mFramesSentBytes
				+ L" reliable=" + (mReliable?L"true":L"false")
				+ L" encrypt=" + (mEncrypt?L"true":L"false")
				+ L" noTransmitCnt" + mNoTransmitCnt
				+ L"}";
		}
	}
}