/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/Transport/PhotonTransport.h"
#include "PhotonVoice-cpp/inc/VoiceClient.h"

/** @file PhotonVoice-cpp/inc/Transport/PhotonTransport.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		namespace EventSubcode
		{
			static const nByte VOICE_INFO = 1;
			static const nByte VOICE_REMOVE = 2;
			//static const nByte FRAME = 3;
		}

		namespace EventParam
		{
			static const nByte VOICE_ID = 1;
			static const nByte SAMPLING_RATE = 2;
			static const nByte CHANNELS = 3;
			static const nByte FRAME_DURATION_US = 4;
			static const nByte BITRATE = 5;
			static const nByte USER_DATA = 10;
			static const nByte EVENT_NUMBER = 11;
			static const nByte CODEC = 12;
		}

		PhotonTransportProtocol::PhotonTransportProtocol(const ILogger& logger)
			: mLogger(logger)
		{
		}

		PhotonTransportProtocol::~PhotonTransportProtocol(void)
		{
		}

		Object PhotonTransportProtocol::buildVoicesInfo(const JVector<const LocalVoice*>& voicesToSend, bool logInfo) const
		{
			Object* infos = allocateArray<Object>(voicesToSend.getSize());
			for(unsigned int i=0; i<voicesToSend.getSize(); i++)
			{
				const LocalVoice* v = voicesToSend.getElementAt(i);
				Dictionary<nByte, Object> dict;
				const VoiceInfo& vInfo = v->getInfo();
				dict.put(EventParam::VOICE_ID, ValueObject<nByte>(v->getId()));
				dict.put(EventParam::CODEC, ValueObject<int>(vInfo.getCodec()));
				dict.put(EventParam::SAMPLING_RATE, ValueObject<int>(vInfo.getSamplingRate()));
				dict.put(EventParam::CHANNELS, ValueObject<int>(vInfo.getChannels()));
				dict.put(EventParam::FRAME_DURATION_US, ValueObject<int>(vInfo.getFrameDurationUs()));
				dict.put(EventParam::BITRATE, ValueObject<int>(vInfo.getBitrate()));
				dict.put(EventParam::USER_DATA, vInfo.getUserData());
				dict.put(EventParam::EVENT_NUMBER, ValueObject<nByte>(v->getEvNumber()));

				infos[i] = ValueObject<Dictionary<nByte, Object> >(dict);
				if(logInfo)
					mLogger.logInfo(v->getLogPrefix() + L" Sending info: " + vInfo.toString() + L" ev=" + v->getEvNumber());
			}

			Object content[] =
			{
				ValueObject<nByte>((nByte)0),
				ValueObject<nByte>(EventSubcode::VOICE_INFO),
				ValueObject<Object*>(infos, static_cast<ArrayLengthType<Object*>::type >(voicesToSend.getSize())),
			};

			ValueObject<Object*> res = ValueObject<Object*>(content, 3);
			deallocateArray(infos);
			return res;
		}

		Object PhotonTransportProtocol::buildVoiceRemoveMessage(const LocalVoice& v) const
		{
			nByte ids[] ={v.getId()};

			Object content[] =
			{
				ValueObject<nByte>((nByte)0),
				ValueObject<nByte>(EventSubcode::VOICE_REMOVE),
				ValueObject<nByte*>(ids, 1),
			};

			mLogger.logInfo(v.getLogPrefix() + L" remove sent");

			return ValueObject<Object*>(content, 3);
		}

		Object PhotonTransportProtocol::buildFrameMessage(nByte voiceId, nByte evNumber, Buffer<nByte> data)
		{
			Object content[] =
			{
				ValueObject<nByte>(voiceId),
				ValueObject<nByte>(evNumber),
				ValueObject<nByte*>(data.getArray(), data.getSize()),
			};
			return ValueObject<Object*>(content, 3);
		}

		// Payloads are arrays. If first array element is 0 than next is event subcode. Otherwise, the event is data frame with voiceId in 1st element.                    
		void PhotonTransportProtocol::onVoiceEvent(VoiceClient& voiceClient, const Object& content, int channelId, int playerId, int localPlayerId)
		{			
			Object* data = *static_cast<const ValueObject<Object*>&>(content).getDataAddress();
			nByte voiceId = ValueObject<nByte>(data[0]).getDataCopy();
			nByte byte1 = ValueObject<nByte>(data[1]).getDataCopy();

			if(!voiceId) // control event
			{
				switch(byte1) // event subcode
				{
				case EventSubcode::VOICE_INFO:
					onVoiceInfo(voiceClient, channelId, playerId, data[2]);
					break;
				case EventSubcode::VOICE_REMOVE:
					onVoiceRemove(voiceClient, channelId, playerId, data[2]);
					break;
				default:
					mLogger.logError(JString(L"[PV] Unknown event subcode ") + byte1);
					break;
				}
			}
			else // stream packet
			{
				nByte evNumber = byte1;
				const ValueObject<nByte*>& receivedBytesObj = (const ValueObject<nByte*>&)data[2];
				Buffer<nByte> receivedBytes(receivedBytesObj.getSizes()[0]);
				memcpy(receivedBytes.getArray(), *receivedBytesObj.getDataAddress(), receivedBytes.getSize());
				voiceClient.onFrame(channelId, playerId, voiceId, evNumber, receivedBytes, playerId == localPlayerId);
			}
		}

		void PhotonTransportProtocol::onVoiceInfo(VoiceClient& voiceClient, int channelId, int playerId, const Object& payload)
		{
			//mpLogger.logInfo(JString("====== ") + payload.toString());
			const ValueObject<Object*>& voiceInfos = static_cast<const ValueObject<Object*>&>(payload);
			Object* voiceInfosArr = *voiceInfos.getDataAddress();
			for(int i=0; i<voiceInfos.getSizes()[0]; i++)
			{
				Dictionary<nByte, Object>* el = static_cast<const ValueObject<Dictionary<nByte, Object> >&>(voiceInfosArr[i]).getDataAddress();
				nByte voiceId = ValueObject<nByte>(el->getValue(EventParam::VOICE_ID)).getDataCopy();
				nByte eventNumber = ValueObject<nByte>(el->getValue(EventParam::EVENT_NUMBER)).getDataCopy();
				VoiceInfo info = createVoiceInfoFromEventPayload(*el);
				voiceClient.onVoiceInfo(channelId, playerId, voiceId, eventNumber, info);
			}
		}

		void PhotonTransportProtocol::onVoiceRemove(VoiceClient& voiceClient, int channelId, int playerId, const Object& payload)
		{
			const ValueObject<nByte*>& voiceIds = (const ValueObject<nByte*>&)payload;
			voiceClient.onVoiceRemove(channelId, playerId, JVector<nByte>(*voiceIds.getDataAddress(), voiceIds.getSizes()[0]));
		}

		VoiceInfo PhotonTransportProtocol::createVoiceInfoFromEventPayload(const Dictionary<nByte, Object>& h)
		{
			return VoiceInfo()
				.setCodec(ValueObject<int>(h.getValue(EventParam::CODEC)).getDataCopy())
				.setSamplingRate(ValueObject<int>(h.getValue(EventParam::SAMPLING_RATE)).getDataCopy())
				.setChannels(ValueObject<int>(h.getValue(EventParam::CHANNELS)).getDataCopy())
				.setFrameDurationUs(ValueObject<int>(h.getValue(EventParam::FRAME_DURATION_US)).getDataCopy())
				.setBitrate(ValueObject<int>(h.getValue(EventParam::BITRATE)).getDataCopy())
				.setUserData(*h.getValue(EventParam::USER_DATA))
				;
		}
	}
}