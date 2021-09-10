/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/OpusCodec.h"
#include "PhotonVoice-cpp/inc/VoiceClient.h"

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		RemoteVoice::RemoteVoice(VoiceClient* pClient, const RemoteVoiceOptions& options, int channelId, int playerId, nByte voiceId, const VoiceInfo& info, nByte lastEventNumber)
			: mpVoiceClient(pClient)
			, mInfo(info)
			, mOptions(options)
			, mChannelId(channelId)
			, mPlayerId(playerId)
			, mVoiceId(voiceId)
			, mDecodeThreadExit(false)
			, mDecodeThreadExited(false)
			, mLastEvNumber(lastEventNumber)
		{
			Thread::create(decodeThreadStarter, this);
		}

		RemoteVoice::~RemoteVoice(void)
		{
			mDecodeThreadExit = true;
			mFrameQueueReady.set(); // let decodeThread dispose resources and exit
			while(true)
			{
				Lockguard lock(mDecodeThreadLock);
				if(mDecodeThreadExited)
					break;
			}

			if(mOptions.mpRemoteVoiceRemoveAction)
				mOptions.mpRemoteVoiceRemoveAction(mOptions.mpRemoteVoiceRemoveActionOpaque);
		}

		const VoiceInfo& RemoteVoice::getInfo(void) const
		{
			return mInfo;
		}

		const RemoteVoiceOptions& RemoteVoice::getOptions(void) const
		{
			return mOptions;
		}

		int RemoteVoice::getChannelId(void) const
		{
			return mChannelId;
		}

		JString RemoteVoice::getName(void) const
		{
			return JString(L"Remote v#") + mVoiceId + " ch#" + mpVoiceClient->channelStr(mChannelId) + " p#" + mPlayerId;
		}

		JString RemoteVoice::getLogPrefix(void) const
		{
			return JString(L"[PV] ") + getName();
		}

		static nByte byteDiff(nByte latest, nByte last)
		{
			return (nByte)(latest - (last + 1));
		}

		void RemoteVoice::receiveBytes(Buffer<nByte> receivedBytes, nByte evNumber)
		{
			// receive-gap detection and compensation
			if(evNumber != mLastEvNumber) // skip check for 1st event 
			{
				int missing = byteDiff(evNumber, mLastEvNumber);
				if(missing)
					mpVoiceClient->getTransport()->logDebug(getLogPrefix() + L" evNumer: " + evNumber + L" playerVoice.lastEvNumber: " + mLastEvNumber + L" missing: " + missing + L" r/b " + receivedBytes.getSize());

				mLastEvNumber = evNumber;

				// restoring missing frames
				receiveNullFrames(missing);

				mpVoiceClient->mFramesLost += missing;
			}
			receiveFrame(receivedBytes);
		}

		void RemoteVoice::receiveFrame(Buffer<nByte> frame)
		{
			{
				Lockguard lock(mFrameQueue);
				mFrameQueue.enqueue(frame);
			}
			mFrameQueueReady.set();
		}

		void RemoteVoice::receiveNullFrames(int count)
		{
			{
				Lockguard lock(mFrameQueue);
				for(int i=0; i<count; i++)
					mFrameQueue.enqueue(Buffer<nByte>::getEmpty());
			}
			mFrameQueueReady.set();
		}

		void RemoteVoice::decodeThreadStarter(void* args)
		{
			RemoteVoice* v = static_cast<RemoteVoice*>(args);
			v->decodeThread();
		}

		void RemoteVoice::decodeThread(void)
		{
			SharedPointer<IDecoder> decoder;
			if(!mOptions.mspDecoder)
				decoder = createDefaultDecoder();
			else
			{
				if(mOptions.mOutputType == RemoteVoiceOptions::OutputType::None)
					decoder = mOptions.mspDecoder;
				else
				{
					decoder = static_cast<IDecoder*>(NULL);
					mpVoiceClient->getTransport()->logDebug(getLogPrefix() + L": Setting decoder output with RemoteVoiceOptions setOutput is not supported for custom decoders (set via RemoteVoiceOptions Decoder property). Assign output directly to decoder (output type = " + mOptions.mOutputType + ")");
				}
			}

			if(!decoder)
			{
				mDecodeThreadExited = true;
				return;
			}

			mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Starting decode thread");
			decoder->open(mInfo);

			while(!decoder->getError().length())
			{
				mFrameQueueReady.waitOne(); // Wait until data is pushed to the queue or Dispose signals.
				Lockguard lock(mDecodeThreadLock);
				if(mDecodeThreadExit)
					break;

				bool empty = false;
				while(!empty) // Dequeue and process while the queue is not empty
				{
					Buffer<nByte> f;
					{
						Lockguard lock(mFrameQueue);
						if(mFrameQueue.getSize() > 0)
							f = mFrameQueue.dequeue();
						else
							empty = true;
					}
					if(!empty)
						decoder->input(f);
				}
			}
			if(decoder->getError().length())
				mpVoiceClient->getTransport()->logError(getLogPrefix() + L": Error in decode thread: " + decoder->getError());

			{
				Lockguard lock(mFrameQueue);
				mFrameQueue.removeAllElements();
			}

			mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Exiting decode thread");

			Lockguard lock(mDecodeThreadLock);
			mDecodeThreadExited = true;
		}

		IDecoder* RemoteVoice::createDefaultDecoder(void)
		{
			switch(mOptions.mOutputType)
			{
			case RemoteVoiceOptions::OutputType::Float:
				if(mInfo.getCodec() == Codec::AUDIO_OPUS)
				{
					mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Creating default decoder for output type = " + mOptions.mOutputType);
					return allocate<Opus::Decoder<float> >(mOptions.mpOutputOpaque, mOptions.mOutput.mpFloatOutput, *mpVoiceClient->getTransport());
				}
				else
				{
					mpVoiceClient->getTransport()->logError(getLogPrefix() + L": Action<float[]> output set for not audio decoder (output type = " + mOptions.mOutputType + ")");
					return NULL;
				}
			case RemoteVoiceOptions::OutputType::Short:
				if(mInfo.getCodec() == Codec::AUDIO_OPUS)
				{
					mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Creating default decoder for output type = " + mOptions.mOutputType);
					return allocate<Opus::Decoder<short> >(mOptions.mpOutputOpaque, mOptions.mOutput.mpShortOutput, *mpVoiceClient->getTransport());
				}
				else
				{
					mpVoiceClient->getTransport()->logError(getLogPrefix() + L": Action<short[]> output set for not audio decoder (output type = " + mOptions.mOutputType + ")");
					return NULL;
				}
#if EG_PHOTON_VOICE_VIDEO_ENABLE
			case RemoteVoiceOptions::OutputType::Image:
				if(mInfo.getCodec() == Codec::VIDEO_VP8)
				{
					mpVoiceClient->getTransport()->logInfo(getLogPrefix() + L": Creating default decoder for output type = " + mOptions.mOutputType);
					IDecoderQueuedOutputImage* vd = NULL;
					//var vd = new VPxCodec.Decoder(mOptions.output as Action<ImageOutputBuf>);
					if (mOptions.mOutputImageFormat != ImageFormat::Undefined)
						vd->setOutputImageFormat(mOptions.mOutputImageFormat);
					if (mOptions.mOutputImageFlip != Flip::Undefined)
						vd->setOutputImageFlip(mOptions.mOutputImageFlip);
					return vd;
				}
				else
				{
					mpVoiceClient->getTransport()->logError(getLogPrefix() + L": Action<ImageOutputBuf> output set for not video decoder (output type = " + mOptions.mOutputType + ")");
					return NULL;
				}
#endif
			case RemoteVoiceOptions::OutputType::None:
			default:
				mpVoiceClient->getTransport()->logError(getLogPrefix() + L": Output must be set in RemoteVoiceOptions with setOutput call (output type = " + mOptions.mOutputType + ")");
				return NULL;
			}
		}

		JString& RemoteVoice::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"info=" + mInfo.toString(withTypes)
				+ L" options=" + mOptions.toString(withTypes)
				+ L" channelId=" + mChannelId
				+ L" playerId=" + mPlayerId
				+ L" voiceId=" + mVoiceId
				+ L"}";
		}
	}
}