/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Photon-cpp/inc/Enums/EventCode.h"
#include "Photon-cpp/inc/Enums/ReceiverGroup.h"
#include "LoadBalancing-cpp/inc/Internal/Enums/EventCode.h"
#include "PhotonVoice-cpp/inc/Transport/LoadBalancingTransport.h"

/** @file PhotonVoice-cpp/inc/Transport/LoadBalancingTransport.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;
		using namespace LoadBalancing;
		using namespace LoadBalancing::Internal;

		static const int VOICE_CHANNEL = 0;

		LockableNonAssignable<Logger> LoadBalancingTransport::mLogger;



		LoadBalancingTransport::LoadBalancingTransport(Client& lbClient, BaseListener& debugReturnListener, bool direct)
			: mLBClient(lbClient)			
			, mpProtocol(NULL)
			, mDirect(direct)
		{
			setListener(&debugReturnListener);
			mpProtocol = allocate<PhotonTransportProtocol>(*this);
			if(lbClient.getChannelCountUserChannels() < Codec::count() + 1) // channel per stream type, channel 0 is for user events
				logError(JString(L"[PV] Need ") + (Codec::count() + 1) + L" LoadBalancing channels but have only %d" + lbClient.getChannelCountUserChannels());
		}

		LoadBalancingTransport::~LoadBalancingTransport(void)
		{
			deallocate(mpProtocol);
		}

		void LoadBalancingTransport::logError(const JString& msg) const
		{
			std::lock_guard<std::mutex> lock(mLogger);
			EGLOG(DebugLevel::ERRORS, msg);
		}

		void LoadBalancingTransport::logWarning(const JString& msg) const
		{
			std::lock_guard<std::mutex> lock(mLogger);
			EGLOG(DebugLevel::WARNINGS, msg);
		}

		void LoadBalancingTransport::logInfo(const JString& msg) const
		{
			std::lock_guard<std::mutex> lock(mLogger);
			EGLOG(DebugLevel::INFO, msg);
		}

		void LoadBalancingTransport::logDebug(const JString& msg) const
		{
			std::lock_guard<std::mutex> lock(mLogger);
			EGLOG(DebugLevel::ALL, msg);
		}

		// send different media type to different channels for efficiency
		nByte LoadBalancingTransport::photonChannelForCodec(int codec) const
		{
			for(size_t i=0; i<Codec::count(); i++)
				if(Codec::List[i] == codec)
					return static_cast<nByte>(1+i); // 0 is for user events
			// should never happen if voice initialized properly with available codec
			return 0;
		}

		bool LoadBalancingTransport::isChannelJoined(int /*channelId*/)
		{
			return mLBClient.getIsInRoom();
		}

		/// <summary>
		/// This method dispatches all available incoming commands and then sends this client's outgoing commands.
		/// Call this method regularly (2 to 20 times a second).
		/// </summary>
		void LoadBalancingTransport::service(void)
		{
			sendFrameRaiseEvents();			
		}

		void LoadBalancingTransport::setDebugEchoMode(const LocalVoice& v)
		{
			if(mLBClient.getIsInRoom())
			{
				if(v.getDebugEchoMode())
				{
					JVector<const LocalVoice*> voices;
					voices.addElement(&v);
					sendVoicesInfo(voices, v.getChannelId(), mLBClient.getLocalPlayer().getNumber());
				}
				else
					sendVoiceRemove(v, v.getChannelId(), mLBClient.getLocalPlayer().getNumber());
			}
		}

		/// <summary>
		/// Set global audio group for this client. This call sets InterestGroup for existing local voices and for created later to given value.
		/// Client set as listening to this group only until LoadBalancingPeer.OpChangeGroups() called. This method can be called any time.
		/// </summary>
		/// <see cref="LocalVoice.InterestGroup"/>
		/// <see cref="LoadBalancingPeer.OpChangeGroups(byte[], byte[])"/>
		nByte LoadBalancingTransport::getGlobalInterestGroup(void) const
		{
			return mpVoiceClient->getGlobalInterestGroup();
		}

		void LoadBalancingTransport::setGlobalInterestGroup(nByte value)
		{
			mpVoiceClient->setGlobalInterestGroup(value);
			if(mLBClient.getIsInRoom())
			{
				JVector<nByte> remove;
				if(mpVoiceClient->getGlobalInterestGroup())
				{
					JVector<nByte> add(&value, 1);
					mLBClient.opChangeGroups(&remove, &add);
				}
				else
					mLBClient.opChangeGroups(&remove, NULL);
			}
		}

		void LoadBalancingTransport::sendVoicesInfo(const JVector<const LocalVoice*>& voices, int channelId, int targetPlayerId)
		{
			Object content = mpProtocol->buildVoicesInfo(voices, true);
			RaiseEventOptions opt;
			if(targetPlayerId)
				opt.setTargetPlayers(&targetPlayerId, 1);

			mLBClient.opRaiseEvent(true, content, EventCode::VOICE_DATA, opt);

			if(!targetPlayerId) // send debug echo infos to myself if broadcast requested
				sendDebugEchoVoicesInfo(channelId);
		}

		static void sendDebugEchoVoicesInfoOp(void* opaque, const LocalVoice& v)
		{
			JVector<const LocalVoice*>* res = static_cast<JVector<const LocalVoice*>*>(opaque);
			if(v.getDebugEchoMode())
				res->addElement(&v);
		}

		/// <summary>Send VoicesInfo events to the local player for all voices that have DebugEcho enabled.</summary>
		/// This function will call <see cref="SendVoicesInfo"></see> for all local voices of our <see cref="VoiceClient"></see>
		/// that have DebugEchoMode set to true, with the given channel ID, and the local Player's ActorNumber as target.
		/// <param name="channelId">Transport Channel ID</param>
		void LoadBalancingTransport::sendDebugEchoVoicesInfo(int channelId)
		{
			JVector<const LocalVoice*> voices;
			//voices.addElement(&v);
			mpVoiceClient->forEachLocalVoice(&voices, &sendDebugEchoVoicesInfoOp);
			if(voices.getSize() > 0)
				sendVoicesInfo(voices, channelId, mLBClient.getLocalPlayer().getNumber());
		}

		void LoadBalancingTransport::sendVoiceRemove(const LocalVoice& voice, int /*channelId*/, int targetPlayerId)
		{
			Object content = mpProtocol->buildVoiceRemoveMessage(voice);

			RaiseEventOptions opt;
			if(targetPlayerId)
				opt.setTargetPlayers(&targetPlayerId, 1);
			if(voice.getDebugEchoMode())
				opt.setReceiverGroup(Lite::ReceiverGroup::ALL);
		
			mLBClient.opRaiseEvent(true, content, EventCode::VOICE_DATA, opt);
		}

		// Raise event from decoding thread. Requires LoadBalancing client calls synchronization

		//void LoadBalancingTransport::SendFrame(Buffer<nByte> data, nByte evNumber, nByte voiceId, int channelId, const LocalVoice* localVoice)
		//{
		//	Object content[3];
		//	content[0] = ValueObject<nByte>(voiceId);
		//	content[1] = ValueObject<nByte>(evNumber);
		//	content[2] = ValueObject<nByte*>(data.getArray(), data.getSize());

		//	RaiseEventOptions opt;
		//	opt.setChannelID(channelId);
		//	//Encrypt = localVoice.Encrypt
		//	if(localVoice->getDebugEchoMode())
		//		opt.setReceiverGroup(Lite::ReceiverGroup::ALL);
		//	opt.setInterestGroup(localVoice->getInterestGroup());

		//	lbClient->opRaiseEvent(localVoice->getReliable(), content, 3, VoiceEventCode::GetCode(channelId), opt);
		//	lbClient->sendOutgoingCommands();
		//}

		// Push outgoing frame event to the queue...
		void LoadBalancingTransport::sendFrame(Buffer<nByte> data, nByte evNumber, nByte voiceId, int /* channelId: voice channels not supported */, const LocalVoice& localVoice)
		{
			nByte photonChannelId = photonChannelForCodec(localVoice.getInfo().getCodec());
			SendFrameEventParams p;
			p.mData = data;
			p.mEvNumber = evNumber;
			p.mVoiceId= voiceId;
			p.mChannelId= photonChannelId;
			p.mReliable = localVoice.getReliable();
			p.mInterestGroup = localVoice.getInterestGroup();
			p.mDebugEchoMode = localVoice.getDebugEchoMode();

			Lockguard lock(mSendFrameRaiseEventQueue);
			mSendFrameRaiseEventQueue.enqueue(p);
		}

		// ... then send frame event in main thread
		void LoadBalancingTransport::sendFrameRaiseEvents(void)
		{
			bool empty = false;
			while(!empty) // Dequeue and process while the queue is not empty
			{
				SendFrameEventParams p;
				{
					Lockguard lock(mSendFrameRaiseEventQueue);
					if(mSendFrameRaiseEventQueue.getSize() > 0)
						p = mSendFrameRaiseEventQueue.dequeue();
					else
						empty = true;
				}
				if(!empty)
				{
					Object content[3];
					content[0] = ValueObject<nByte>(p.mVoiceId);
					content[1] = ValueObject<nByte>(p.mEvNumber);
					content[2] = ValueObject<nByte*>(p.mData.getArray(), p.mData.getSize());
					if(mDirect)
						mLBClient.sendDirect(content, 3, JVector<int>(), true);
					else
					{
						RaiseEventOptions opt;
						opt.setChannelID(p.mChannelId);
						//Encrypt = localVoice.Encrypt
						if(p.mDebugEchoMode)
							opt.setReceiverGroup(Lite::ReceiverGroup::ALL);
						opt.setInterestGroup(p.mInterestGroup);
						mLBClient.opRaiseEvent(p.mReliable, content, 3, EventCode::VOICE_DATA, opt);
						mLBClient.sendOutgoingCommands();
					}
				}
			}
		}

		JString LoadBalancingTransport::channelIdStr(int /*channelId*/)
		{
			return JString();
		}

		JString LoadBalancingTransport::playerIdStr(int /*playerId*/)
		{
			return JString();
		}

		void LoadBalancingTransport::onCustomEvent(int playerNr, nByte eventCode, const Object& eventContent)
		{
			if(eventCode == EventCode::VOICE_DATA)
				onVoiceEvent(playerNr, eventContent);
		}

		void LoadBalancingTransport::onDirectMessage(const Object& msg, int playerNr, bool /*relay*/)
		{
			onVoiceEvent(playerNr, msg);
		}

		void LoadBalancingTransport::onVoiceEvent(int playerNr, const Object& eventContent)
		{
			mpProtocol->onVoiceEvent(*mpVoiceClient, eventContent, VOICE_CHANNEL, playerNr, mLBClient.getLocalPlayer().getNumber());
		}

		void LoadBalancingTransport::onJoinRoom(void)
		{
			mpVoiceClient->onJoinChannel(VOICE_CHANNEL);

			if(mpVoiceClient->getGlobalInterestGroup())
			{
				JVector<nByte> toRemove;
				JVector<nByte> toAdd;
				toAdd.addElement(mpVoiceClient->getGlobalInterestGroup());
				mLBClient.opChangeGroups(&toRemove, &toAdd);
			}
		}

		void LoadBalancingTransport::onLeaveRoom(void)
		{
			mpVoiceClient->onLeaveChannel(VOICE_CHANNEL);
		}

		void LoadBalancingTransport::onPlayerJoinRoom(int playerNr)
		{			
			if(playerNr != mLBClient.getLocalPlayer().getNumber())
				mpVoiceClient->onPlayerJoin(VOICE_CHANNEL, playerNr);
		}

		void LoadBalancingTransport::onPlayerLeaveRoom(int playerNr)
		{
			if(playerNr == mLBClient.getLocalPlayer().getNumber())
				mpVoiceClient->onLeaveChannel(VOICE_CHANNEL);
			else
				mpVoiceClient->onPlayerLeave(VOICE_CHANNEL, playerNr);
		}

		JString& LoadBalancingTransport::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"logger=" + mLogger.toString(withTypes)
				+ L" direct=" + (mDirect?L"true":L"false")
				+ L" sendFrameRaiseEventQueue=" + mSendFrameRaiseEventQueue.toString(withTypes)
				+ L"}";
		}

		JString& LoadBalancingTransport::SendFrameEventParams::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"data=" + mData.toString(withTypes)
				+ L" evNumber=" + mEvNumber
				+ L" voiceId=" + mVoiceId
				+ L" channelId=" + mChannelId
				+ L" reliable=" + (mReliable?L"true":L"false")
				+ L" interestGroup=" + mInterestGroup
				+ L" debugEchoMode=" + mDebugEchoMode
				+ L"}";
		}
	}
}