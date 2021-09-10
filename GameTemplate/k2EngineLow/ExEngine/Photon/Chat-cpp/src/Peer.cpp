/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Peer.h"
#include "Chat-cpp/inc/Internal/Enums/OperationCode.h"
#include "Chat-cpp/inc/Internal/Enums/ParameterCode.h"

/** @file Chat-cpp/inc/Peer.h */

namespace ExitGames
{
	namespace Chat
	{
		using namespace Common;
		using namespace Common::MemoryManagement;
		using namespace Internal;

		Peer::Peer(Photon::PhotonListener& listener, nByte connectionProtocol)
			: PhotonPeer(listener, connectionProtocol)
		{
		}

		Peer::~Peer(void)
		{
		}

		bool Peer::opAuthenticateOnNameServer(const JString& appID, const JString& appVersion, const JString& region, const AuthenticationValues& authenticationValues)
		{
			Photon::OperationRequestParameters op;
			op.put(ParameterCode::APPLICATION_ID, ValueObject<JString>(appID));
			op.put(ParameterCode::APP_VERSION, ValueObject<JString>(appVersion));
			op.put(ParameterCode::REGION, ValueObject<JString>(region));
			if(authenticationValues.getUserID().length())
				op.put(ParameterCode::USER_ID, ValueObject<JString>(authenticationValues.getUserID()));
			if(authenticationValues.getType() != CustomAuthenticationType::NONE)
			{
				op.put(ParameterCode::CLIENT_AUTHENTICATION_TYPE, ValueObject<nByte>(authenticationValues.getType()));
				if(authenticationValues.getSecret().length())
					op.put(ParameterCode::SECRET, ValueObject<JString>(authenticationValues.getSecret()));
				else
				{
					if(authenticationValues.getParameters().length())
						op.put(ParameterCode::CLIENT_AUTHENTICATION_PARAMETERS, ValueObject<JString>(authenticationValues.getParameters()));
					if(authenticationValues.getData().getType() != TypeCode::EG_NULL)
						op.put(ParameterCode::CLIENT_AUTHENTICATION_DATA, authenticationValues.getData());
				}
			}
			EGLOG(DebugLevel::INFO, Photon::OperationRequest(OperationCode::AUTHENTICATE, op).toString(true));
			// TODO: use IsPayloadEncryptionAvailable
			return opCustom(Photon::OperationRequest(OperationCode::AUTHENTICATE, op), true, 0, true);
		}

		bool Peer::opAuthenticateOnFrontEnd(const JString& secret)
		{
			Photon::OperationRequestParameters op;
			op.put(ParameterCode::SECRET, ValueObject<JString>(secret));
			EGLOG(DebugLevel::INFO, Photon::OperationRequest(OperationCode::AUTHENTICATE, op).toString(true));
			// TODO: use IsPayloadEncryptionAvailable
			return opCustom(Photon::OperationRequest(OperationCode::AUTHENTICATE, op), true, 0, false);
		}

		bool Peer::opSubscribe(const Common::JVector<Common::JString>& channels, int messagesFromHistory)
		{
			return sendChannelOperation(channels, OperationCode::SUBSCRIBE, messagesFromHistory);
		}

		bool Peer::opUnsubscribe(const Common::JVector<Common::JString>& channels)
		{
			return sendChannelOperation(channels, OperationCode::UNSUBSCRIBE);
		}

		bool Peer::opPublishMessage(const JString& channelName, const Object& message)
		{
			Photon::OperationRequestParameters op;
			op.put(ParameterCode::CHANNEL, ValueObject<JString>(channelName));
			op.put(ParameterCode::MESSAGE, message);
			return opCustom(Photon::OperationRequest(OperationCode::PUBLISH, op), true);
		}

		bool Peer::opSendPrivateMessage(const JString& userName, const Object& message, bool encrypt)
		{
			Photon::OperationRequestParameters op;
			op.put(ParameterCode::USER_ID, ValueObject<JString>(userName));
			op.put(ParameterCode::MESSAGE, message);
			return opCustom(Photon::OperationRequest(OperationCode::SEND_PRIVATE, op), true, 0, encrypt);
		}

		bool Peer::opSetOnlineStatus(int status)
		{
			return opSetOnlineStatus(status, Object(), true);
		}

		bool Peer::opSetOnlineStatus(int status, const Object& message, bool skipMessage)
		{
			Photon::OperationRequestParameters op;
			op.put(ParameterCode::STATUS, ValueObject<int>(status));
			if(skipMessage)
				op.put(ParameterCode::SKIP_MESSAGE, ValueObject<bool>(true));
			else
				op.put(ParameterCode::MESSAGE, message);
			return opCustom(Photon::OperationRequest(OperationCode::UPDATE_STATUS, op), true);
		}

		bool Peer::opAddFriends(const Common::JVector<Common::JString>& vUserIDs)
		{
			Photon::OperationRequestParameters op;
			unsigned int size = vUserIDs.getSize();
			JString* friends = allocateArray<JString>(size);
			for(unsigned int i=0; i<size; ++i)
				friends[i] = vUserIDs.getElementAt(i);

			op.put(ParameterCode::FRIENDS, ValueObject<const JString*>(friends, size));
			bool res = opCustom(Photon::OperationRequest(OperationCode::ADD_FRIENDS, op), true);
			deallocateArray(friends);
			return res;
		}

		bool Peer::opRemoveFriends(const Common::JVector<Common::JString>& vUserIDs)
		{
			Photon::OperationRequestParameters op;
			unsigned int size = vUserIDs.getSize();
			JString* friends = allocateArray<JString>(size);
			for(unsigned int i=0; i<size; ++i)
				friends[i] = vUserIDs.getElementAt(i);

			op.put(ParameterCode::FRIENDS, ValueObject<const JString*>(friends, size));
			bool res = opCustom(Photon::OperationRequest(OperationCode::REMOVE_FRIENDS, op), true);
			deallocateArray(friends);
			return res;
		}

		bool Peer::sendChannelOperation(const JVector<JString>& vChannels, nByte operation, int messagesFromHistory)
		{
			Photon::OperationRequestParameters op;
			unsigned int size = vChannels.getSize();
			JString* channels = allocateArray<JString>(size);
			for(unsigned int i=0; i<size; ++i)
				channels[i] = vChannels.getElementAt(i);

			op.put(ParameterCode::CHANNELS, ValueObject<const JString*>(channels, size));
			if(messagesFromHistory)
				op.put(ParameterCode::HISTORY_LENGTH, ValueObject<int>(messagesFromHistory));

			bool res = opCustom(Photon::OperationRequest(operation, op), true);

			deallocateArray(channels);
			return res;
		}
	}
}