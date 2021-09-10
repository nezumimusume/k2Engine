/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceClient.h"

/** @file PhotonVoice-cpp/inc/VoiceClient.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		/// <summary>Lost frames counter.</summary>
		int VoiceClient::getFramesLost(void) const
		{
			return mFramesLost;
		}

		/// <summary>Received frames counter.</summary>
		int VoiceClient::getFramesReceived(void) const
		{
			return mFramesReceived;
		}

		/// <summary>Sent frames counter.</summary>
		int VoiceClient::getFramesSent(void) const
		{
			int x = 0;
			mLocalVoicesLock.lock();
			for(unsigned int i=0; i<mLocalVoices.getSize(); i++)
				x += mLocalVoices[i]->getFramesSent();
			mLocalVoicesLock.unlock();
			return x;
		}

		/// <summary>Sent frames bytes counter.</summary>
		int VoiceClient::getFramesSentBytes(void) const
		{
			int x = 0;
			mLocalVoicesLock.lock();
			for(unsigned int i = 0; i<mLocalVoices.getSize(); i++)
				x += mLocalVoices[i]->getFramesSentBytes();
			mLocalVoicesLock.unlock();
			return x;
		}

		/// <summary>Average time required voice packet to return to sender.</summary>
		int VoiceClient::getRoundTripTime(void) const
		{
			return mRoundTripTime;
		}

		/// <summary>Average round trip time variation.</summary>
		int VoiceClient::getRoundTripTimeVariance(void) const
		{
			return mRoundTripTimeVariance;
		}

		/// <summary>Do not log warning when duplicate info received.</summary>
		bool VoiceClient::getSuppressInfoDuplicateWarning(void) const
		{
			return mSuppressInfoDuplicateWarning;
		}

		void VoiceClient::setSuppressInfoDuplicateWarning(bool yes)
		{
			mSuppressInfoDuplicateWarning = yes;
		}

		/// <summary>
		/// Register a method to be called when remote voice info arrived (after join or new new remote voice creation).
		/// Metod parameters: (int channelId, int playerId, byte voiceId, VoiceInfo voiceInfo, ref RemoteVoiceOptions options);
		/// </summary>
		void VoiceClient::setOnRemoteVoiceInfoAction(void* opaque, VoiceClient::RemoteVoiceInfoCallback* callback)
		{
			mpOnRemoteVoiceInfoAction = callback;
			mpOnRemoteVoiceInfoActionOpaque = opaque;
		}

		/// <summary>Lost frames simulation ratio.</summary>
		int VoiceClient::getDebugLostPercent(void) const
		{
			return mDebugLostPercent;
		}

		void VoiceClient::setDebugLostPercent(int x)
		{
			mDebugLostPercent = x;
		}

		void VoiceClient::forEachLocalVoice(void* opaque, void(*op)(void*, const LocalVoice&)) const
		{
			mLocalVoicesLock.lock();
			for(unsigned int i = 0; i<mLocalVoices.getSize(); i++)
				op(opaque, *mLocalVoices[i]);
			mLocalVoicesLock.unlock();
		}

		/// <summary>Returns copies of all local voices for given channel.</summary>
		void VoiceClient::forEachLocalVoiceInChannel(int channelId, void* opaque, void(*op)(void*, const LocalVoice&)) const
		{
			mLocalVoicesLock.lock();
			const JVector<Common::Helpers::SharedPointer<LocalVoice> >* channelVoices = mLocalVoicesPerChannel.getValue(channelId);
			mLocalVoicesLock.unlock();
			if(channelVoices)
				for(unsigned int i=0; i<channelVoices->getSize(); i++)
					op(opaque, *(channelVoices->getElementAt(i)));
		}

		/// <summary>Iterates through all remote voices infos.</summary>
		JVector<RemoteVoiceInfo> VoiceClient::getRemoteVoiceInfos() const
		{
			Lockguard lock(mRemoteVoicesLock);

			JVector<RemoteVoiceInfo> res;
			for(unsigned int i=0; i<mRemoteVoices.getSize(); i++)
			{
				int playerId = mRemoteVoices.getKeys()[i];
				const JDictionary<nByte, SharedPointer<RemoteVoice> >& voices = mRemoteVoices[i];
				for(unsigned int j = 0; j<voices.getSize(); j++)
				{
					nByte voiceId = voices.getKeys()[j];
					const RemoteVoice* voice = voices[j];
					res.addElement(RemoteVoiceInfo(voice->getChannelId(), playerId, voiceId, voice->getInfo()));
				}
			}
			return res;
		}

		/// <summary>Creates VoiceClient instance</summary>
		VoiceClient::VoiceClient(IVoiceTransport* transport)
			: mpTransport(transport)
			, mGlobalInterestGroup(0)
			, mVoiceIdCnt(0)
			, mPrevRtt(0)
			, mFramesLost(0)
			, mFramesReceived(0)
			, mFramesSent(0)
			, mFramesSentBytes(0)
			, mRoundTripTime(0)
			, mRoundTripTimeVariance(0)
			, mSuppressInfoDuplicateWarning(false)
			, mpOnRemoteVoiceInfoAction(NULL)
			, mpOnRemoteVoiceInfoActionOpaque(NULL)
			, mDebugLostPercent(0)
		{
			transport->mpVoiceClient = this;
		}

		/// <summary>
		/// This method dispatches all available incoming commands and then sends this client's outgoing commands.
		/// Call this method regularly (2..20 times a second).
		/// </summary>
		void VoiceClient::service()
		{
			Lockguard lock(mLocalVoicesLock);
			for(unsigned int i=0; i<mLocalVoices.getSize(); i++)
				mLocalVoices[i]->service();
		}

		LocalVoice* VoiceClient::tryAddLocalVoice(LocalVoice* pLocalVoice)
		{			
			Lockguard lock(mLocalVoicesLock);
			if(pLocalVoice->getId())
			{
				pLocalVoice->setupEncoder();
				addLocalVoice(pLocalVoice->getId(), pLocalVoice->getChannelId(), pLocalVoice);
				mpTransport->logInfo(pLocalVoice->getLogPrefix() + L" added enc: " + pLocalVoice->getInfo().toString());
				return pLocalVoice;
			}
			else
			{
				deallocate(pLocalVoice);
				return NULL;
			}
		}

#if EG_PHOTON_VOICE_VIDEO_ENABLE
		/// <summary>
		/// Creates outgoing video stream consuming sequence of image buffers.
		/// </summary>
		/// <param name="voiceInfo">Outgoing stream parameters. Set applicable fields to read them by encoder and by receiving client when voice created.</param>
		/// <param name="channelId">Transport channel specific to transport.</param>
		/// <param name="encoder">Encoder compressing video data. Set to null to use default VP8 implementation.</param>
		/// <returns>Outgoing stream handler.</returns>
		//inline LocalVoiceVideo* VoiceClient::CreateLocalVoiceVideo(VoiceInfo voiceInfo, int channelId, SharedPointer<IEncoder> encoder)
		//{
		//	return static_cast<LocalVoiceVideo*>(createLocalVoice(voiceInfo, channelId, (vId, chId) = > new LocalVoiceVideo(this, encoder, vId, voiceInfo, chId)));
		//}
#endif
		nByte VoiceClient::getNewVoiceId(void)
		{
			// id assigned starting from 1 and up to 255
			nByte newId = 0; // non-zero if successfully assigned
			if(mVoiceIdCnt == 255)
			{
				// try to reuse id
				bool ids[256] ={}; // sets all to false;
				for(unsigned int i=0; i<mLocalVoices.getSize(); i++)
					ids[mLocalVoices[i]->mId] = true;
				// ids[0] is not used
				for(nByte id=1; id /* < 256 */; id++)
				{
					if(!ids[id])
					{
						newId = id;
						break;
					}
				}
			}
			else
			{
				mVoiceIdCnt++;
				newId = mVoiceIdCnt;
			}
			return newId;
		}

		void VoiceClient::addLocalVoice(nByte newId, int channelId, LocalVoice* pV)
		{
			SharedPointer<LocalVoice> spVoice = pV;
			mLocalVoices.put(newId, spVoice);

			JVector<SharedPointer<LocalVoice> >* channelVoices = mLocalVoicesPerChannel.getValue(channelId);
			if(!channelVoices)
			{
				JVector<SharedPointer<LocalVoice> > vec(1);
				vec.addElement(spVoice);
				mLocalVoicesPerChannel.put(channelId, vec);
			}
			else
				channelVoices->addElement(spVoice);

			if(mpTransport->isChannelJoined(channelId))
				mpTransport->sendVoicesInfo(JVector<const LocalVoice*>(&pV, 1), channelId, 0); // broadcast if joined

			pV->setInterestGroup(mGlobalInterestGroup);
		}

		/// <summary>
		/// Removes local voice (outgoing data stream).
		/// <param name="voice">Handler of outgoing stream to be removed.</param>
		/// </summary>
		void VoiceClient::removeLocalVoice(LocalVoice& voice)
		{
			mLocalVoicesLock.lock();

			SharedPointer<LocalVoice> spVoice = *mLocalVoices.getValue(voice.mId);
			mLocalVoices.remove(voice.mId);

			JVector<SharedPointer<LocalVoice> >* pChannelVoices = mLocalVoicesPerChannel.getValue(voice.mChannelId);
			if(pChannelVoices)
			{
				pChannelVoices->removeElement(spVoice); // we can't pass the raw LocalVoice* here, as that would mean creating a temporary SharedPointer instance which would not share ownership with the existing SharedPointer instances

				mLocalVoicesLock.unlock();

				if(mpTransport->isChannelJoined(voice.mChannelId))
					mpTransport->sendVoiceRemove(voice, voice.mChannelId, 0);

				mpTransport->logInfo(voice.getLogPrefix() + L" removed");
			}
			else 
				mLocalVoicesLock.unlock();
		}

		void VoiceClient::sendVoicesInfo(int channelId, int targetPlayerId) const
		{
			sendChannelVoicesInfo(channelId, targetPlayerId);
		}

		void VoiceClient::sendChannelVoicesInfo(int channelId, int /*targetPlayerId*/) const
		{
			mLocalVoicesLock.lock();

			const JVector<SharedPointer<LocalVoice> >* channelVoices = mLocalVoicesPerChannel.getValue(channelId);
			if(channelVoices)
			{
				// const copy of channel voices
				JVector<const LocalVoice*> voices(channelVoices->getSize());
				for(unsigned int i=0; i<channelVoices->getSize(); i++)
					voices.addElement((*channelVoices)[i]);
				
				mLocalVoicesLock.unlock();

				mpTransport->sendVoicesInfo(voices, channelId, 0); // broadcast if joined
			}
			else 
				mLocalVoicesLock.unlock();
		}

		IVoiceTransport* VoiceClient::getTransport(void) const
		{
			return mpTransport;
		}

		nByte VoiceClient::getGlobalInterestGroup(void) const
		{
			return mGlobalInterestGroup;
		}

		void VoiceClient::setGlobalInterestGroup(nByte group)
		{
			mGlobalInterestGroup = group;
		}

		void VoiceClient::clearRemoteVoices(void)
		{
			Lockguard lock(mRemoteVoicesLock);

			mRemoteVoices.removeAllElements();
			mpTransport->logInfo(L"[PV] Remote voices cleared");
		}

		void VoiceClient::clearRemoteVoicesInChannel(int channelId)
		{
			Lockguard lock(mRemoteVoicesLock);

			JVector<int> playerToRemove;
			for(unsigned int i=0; i<mRemoteVoices.getSize(); i++)
			{
				JDictionary<nByte, SharedPointer<RemoteVoice> >& voices = mRemoteVoices[i];
				JVector<nByte> voicesToRemove;
				for(unsigned int j=0; j<voices.getSize(); j++)
					if(voices[j]->getChannelId() == channelId)
						voicesToRemove.addElement(voices.getKeys()[j]);
				for(unsigned int j=0; j<voicesToRemove.getSize(); j++)
					voices.remove(voicesToRemove[j]);
				if(!voices.getSize())
					playerToRemove.addElement(mRemoteVoices.getKeys()[i]);
			}
			for(unsigned int i=0; i<mRemoteVoices.getSize(); i++)
				mRemoteVoices.remove(playerToRemove[i]);

			mpTransport->logInfo(L"[PV] Remote voices for channel " + channelStr(channelId) + L" cleared");
		}

		void VoiceClient::clearRemoteVoicesInChannelForPlayer(int channelId, int playerId)
		{
			Lockguard lock(mRemoteVoicesLock);

			const JDictionary<nByte, SharedPointer<RemoteVoice> >* voices = mRemoteVoices.getValue(playerId);
			if(voices)
			{
				for(unsigned int i=0; i<voices->getSize();i++)
				{
					const RemoteVoice* v = (*voices)[i];
					JVector<nByte> voicesToRemove;
					if(v->getChannelId() == channelId)
						voicesToRemove.addElement(voices->getKeys()[i]);
				}
				mRemoteVoices.remove(playerId);
			}
			mpTransport->logInfo(L"[PV] Remote voices in channel " + channelStr(channelId) + L" for player " + playerId +  L" cleared");
		}

		void VoiceClient::onJoinChannel(int channel)
		{
			sendChannelVoicesInfo(channel, 0);// my join, broadcast
		}

		void VoiceClient::onLeaveChannel(int channel) 
		{
			clearRemoteVoicesInChannel(channel);
		}
		
		void VoiceClient::onLeaveAllChannels()
		{
			clearRemoteVoices();
		}

		void VoiceClient::onPlayerJoin(int channelId, int playerId)
		{
			sendVoicesInfo(channelId, playerId);// send to new joined only
		}

		void VoiceClient::onPlayerLeave(int channelId, int playerId)
		{
			clearRemoteVoicesInChannelForPlayer(channelId, playerId);
		}

		void VoiceClient::onVoiceInfo(int channelId, int playerId, nByte voiceId, nByte eventNumber, const VoiceInfo& info)
		{
			Lockguard lock(mRemoteVoicesLock);

			JDictionary<nByte, SharedPointer<RemoteVoice> >* voices = mRemoteVoices.getValue(playerId);
			if(!voices)
			{
				mRemoteVoices.put(playerId, JDictionary<nByte, SharedPointer<RemoteVoice> >());
				voices = mRemoteVoices.getValue(playerId);
			}
			if(!voices->getValue(voiceId))
			{
				mpTransport->logInfo(L"[PV] ch#" + channelStr(channelId) + L" p#" + playerStr(playerId) + L" v#" + voiceId + L" Info received: " + info.toString() + L" ev=" + eventNumber);

				RemoteVoiceOptions options;
				// create default decoder
				// may be overwritten in OnRemoteVoiceInfoAction call					
				if(mpOnRemoteVoiceInfoAction)
					mpOnRemoteVoiceInfoAction(mpOnRemoteVoiceInfoActionOpaque, channelId, playerId, voiceId, info, options);
				RemoteVoice* pRemoteVoice;
				voices->put(voiceId, ALLOCATE(RemoteVoice, pRemoteVoice, this, options, channelId, playerId, voiceId, info, eventNumber));
			}
			else if(!mSuppressInfoDuplicateWarning)
				mpTransport->logWarning(JString(L"[PV] Info duplicate for voice #") + voiceId + L" of player " + playerStr(playerId) + " at channel " + channelStr(channelId));
		}

		void VoiceClient::onVoiceRemove(int channelId, int playerId, const JVector<nByte>& voiceIds)
		{
			Lockguard lock(mRemoteVoicesLock);

			JDictionary<nByte, SharedPointer<RemoteVoice> >* voices = mRemoteVoices.getValue(playerId);
			if(voices)
			{
				for(unsigned int i=0; i<voiceIds.getSize(); i++)
				{
					nByte voiceId = voiceIds[i];
					const SharedPointer<RemoteVoice>* rv = voices->getValue(voiceId);
					if(rv)
					{
						voices->remove(voiceId);
						mpTransport->logInfo(JString(L"[PV] Remote voice #") + voiceId + L" of player " + playerStr(playerId) + L" at channel " + channelStr(channelId) + L" removed");
					}
					else
						mpTransport->logWarning(JString(L"[PV] Remote voice #") + voiceId + L" of player " + playerStr(playerId) + " at channel " + channelStr(channelId) + " not found when trying to remove");
				}
			}
			else
				mpTransport->logWarning(JString("L[PV] Remote voice list of player ") + playerStr(playerId) + " at channel " + channelStr(channelId) + " not found when trying to remove voice(s)");
		}

		void VoiceClient::onFrame(int channelId, int playerId, nByte voiceId, nByte evNumber, Buffer<nByte> receivedBytes, bool isLocalPlayer)
		{
			if(isLocalPlayer)
			{
				SharedPointer<LocalVoice>* pspV;
				{
					Lockguard lock(mLocalVoicesLock);
					// rtt measurement in debug echo mode
					pspV = mLocalVoices.getValue(voiceId);
				}
				if(pspV)
				{
					const int* pTimestamp;
					{
						Lockguard lock((*pspV)->mEventTimestamps);
						pTimestamp = (*pspV)->mEventTimestamps.getValue(evNumber);
					}
					if(pTimestamp)
					{
						int sendTime = *pTimestamp;
						int rtt = GETTIMEMS() - sendTime;
						int rttvar = rtt - mPrevRtt;
						mPrevRtt = rtt;
						if(rttvar < 0) rttvar = -rttvar;
						mRoundTripTimeVariance = (rttvar + getRoundTripTimeVariance()*19)/20;
						mRoundTripTime = (rtt + getRoundTripTime()*19)/20;
					}
				}
			}

			if(mDebugLostPercent > 0 && (rand()*100/RAND_MAX) < mDebugLostPercent)
			{
				mpTransport->logWarning(L"[PV] Debug Lost Sim: 1 packet dropped");
				return;
			}

			mFramesReceived++;

			mRemoteVoicesLock.lock();
			JDictionary<nByte, SharedPointer<RemoteVoice> >* voices = mRemoteVoices.getValue(playerId);
			if(voices)
			{
				SharedPointer<RemoteVoice>* v = voices->getValue(voiceId);
				mRemoteVoicesLock.unlock();
				if(v)
					(**v).receiveBytes(receivedBytes, evNumber);
				else
					mpTransport->logWarning(JString(L"[PV] Frame event for not initialized voice #") + voiceId + L" of player " + playerStr(playerId) + L" at channel " + channelStr(channelId));
			}
			else
			{
				mRemoteVoicesLock.unlock();
				mpTransport->logWarning(JString(L"[PV] Frame event for voice #") + voiceId + L" of not initialized player " + playerStr(playerId) + L" at channel " + channelStr(channelId));
			}
		}
		
		JString VoiceClient::channelStr(int channelId)
		{
			const JString& str = mpTransport->channelIdStr(channelId);
			if(str.length())
				return JString() + channelId + L"(" + str + L")";
			else
				return JString() + channelId;
		}

		JString VoiceClient::playerStr(int playerId)
		{
			const JString& str = mpTransport->playerIdStr(playerId);

			if(str.length())
				return JString() + playerId + "(" + str + ")";
			else
				return JString() + playerId;
		}

		VoiceClient::~VoiceClient(void)
		{
			Lockguard lock(mLocalVoicesLock);
			Lockguard lock2(mRemoteVoicesLock);
		}
	}
}