/* Exit Games Photon Chat - C++ Client Lib
 * Copyright (C) 2004-2021 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "Chat-cpp/inc/Client.h"
#include "Chat-cpp/inc/Enums/DisconnectCause.h"
#include "Chat-cpp/inc/Enums/ErrorCode.h"
#include "Chat-cpp/inc/Internal/AuthenticationValuesSecretSetter.h"
#include "Chat-cpp/inc/Internal/ChannelFactory.h"
#include "Chat-cpp/inc/Internal/ChannelMessageAdder.h"
#include "Chat-cpp/inc/Internal/ChannelPropertiesSetter.h"
#include "Chat-cpp/inc/Internal/Enums/EventCode.h"
#include "Chat-cpp/inc/Internal/Enums/ParameterCode.h"
#include "Chat-cpp/inc/Internal/Enums/OperationCode.h"
#include "Photon-cpp/inc/Enums/PeerState.h"

/** @file Chat-cpp/inc/Client.h */

namespace ExitGames
{
	namespace Chat
	{
		using namespace Common;
		using namespace Common::MemoryManagement;
		using namespace Photon;
		using namespace Internal;

		const JString Client::M_NAMESERVER = L"ns.photonengine.io"; // default name server address
		const EG_CHAR* const REGION = L"EU"; // default region

		/**
		   @class Client
		   Central class of the %Photon %Chat API to connect, handle channels and messages.
		   @details
		   This class must be instantiated with a Chat::Listener instance to get the callbacks and
		   with application id that is setup as %Photon %Chat application.
		   Integrate it into your game loop by calling service() regularly.
		   Call connect() with an Name Server address. Note: Connect covers multiple
		   messages between this client and the servers. A short workflow will connect you to a %Chat server.
		   Each Chat::Client resembles a user in chat. Before you send messages in any public channel, that channel must be subscribed.
		   Private channels represent private chats and created automatically on private message sent or received.
		   getPublicChannels() returns list of subscribed channels, containing messages and senders.
		   getPrivateChannels() contains all incoming and sent private messages.*/

		/** @copydoc Peer::getServerTimeOffset */ 
		int Client::getServerTimeOffset(void) const
		{
			return mPeer.getServerTimeOffset();
		}

		/** @copydoc Peer::getServerTime */ 
		int Client::getServerTime(void) const
		{
			return mPeer.getServerTime();
		}

		/** @copydoc Peer::getBytesOut */ 
		int Client::getBytesOut(void) const
		{
			return mPeer.getBytesOut();
		}

		/** @copydoc Peer::getBytesIn */ 
		int Client::getBytesIn(void) const
		{
			return mPeer.getBytesIn();
		}

		/** @copydoc Peer::getByteCountCurrentDispatch */ 
		int Client::getByteCountCurrentDispatch(void) const
		{
			return mPeer.getByteCountCurrentDispatch();
		}

		/** @copydoc Peer::getByteCountLastOperation */ 
		int Client::getByteCountLastOperation(void) const
		{
			return mPeer.getByteCountLastOperation();
		}

		/** @copydoc Peer::getSentCountAllowance */ 
		int Client::getSentCountAllowance(void) const
		{
			return mPeer.getSentCountAllowance();
		}

		/** @copydoc Peer::setSentCountAllowance */ 
		void Client::setSentCountAllowance(int sentCountAllowance)
		{
			mPeer.setSentCountAllowance(sentCountAllowance);
		}

		/** @copydoc Peer::getTimePingInterval */ 
		int Client::getTimePingInterval(void) const
		{
			return mPeer.getTimePingInterval();
		}

		/** @copydoc Peer::setTimePingInterval */ 
		void Client::setTimePingInterval(int timePingInterval)
		{
			mPeer.setTimePingInterval(timePingInterval);
		}

		/** @copydoc Peer::getRoundTripTime */ 
		int Client::getRoundTripTime(void) const
		{
			return mPeer.getRoundTripTime();
		}

		/** @copydoc Peer::getRoundTripTimeVariance */ 
		int Client::getRoundTripTimeVariance(void) const
		{
			return mPeer.getRoundTripTimeVariance();
		}

		/** @copydoc Peer::getTimestampOfLastSocketReceive */ 
		int Client::getTimestampOfLastSocketReceive(void) const
		{
			return mPeer.getTimestampOfLastSocketReceive();
		}

		/** @copydoc Peer::getDebugOutputLevel */ 
		int Client::getDebugOutputLevel(void) const
		{
			return mPeer.getDebugOutputLevel();
		}

		/** @copydoc Peer::setDebugOutputLevel */ 
		bool Client::setDebugOutputLevel(int debugLevel)
		{
			return mLogger.setDebugOutputLevel(debugLevel) && mPeer.setDebugOutputLevel(debugLevel);
		}

		/**
		   @copydoc Peer::getLogFormatOptions() */
		const LogFormatOptions& Client::getLogFormatOptions(void) const
		{
			return mPeer.getLogFormatOptions();
		}

		/**
		   @copydoc Peer::setLogFormatOptions() */
		void Client::setLogFormatOptions(const LogFormatOptions& formatOptions)
		{
			mLogger.setFormatOptions(formatOptions);
			mPeer.setLogFormatOptions(formatOptions);
		}

		/** @copydoc Peer::getIncomingReliableCommandsCount */ 
		int Client::getIncomingReliableCommandsCount(void) const
		{
			return mPeer.getIncomingReliableCommandsCount();
		}

		/** @copydoc Peer::getPeerID */ 
		short Client::getPeerID(void) const
		{
			return mPeer.getPeerID();
		}

		/** @copydoc Peer::getDisconnectTimeout */ 
		int Client::getDisconnectTimeout(void) const
		{
			return mPeer.getDisconnectTimeout();
		}

		/** @copydoc Peer::setDisconnectTimeout */ 
		void Client::setDisconnectTimeout(int disconnectTimeout)
		{
			mPeer.setDisconnectTimeout(disconnectTimeout);
		}

		/** @copydoc Peer::getQueuedIncomingCommands */ 
		int Client::getQueuedIncomingCommands(void) const
		{
			return mPeer.getQueuedIncomingCommands();
		}

		/** @copydoc Peer::getQueuedOutgoingCommands */ 
		int Client::getQueuedOutgoingCommands(void) const
		{
			return mPeer.getQueuedOutgoingCommands();
		}

		/** @copydoc Peer::getIsEncryptionAvailable */ 
		bool Client::getIsPayloadEncryptionAvailable(void) const
		{
			return mPeer.getIsPayloadEncryptionAvailable();
		}

		/** @copydoc Peer::getResentReliableCommands */ 
		int Client::getResentReliableCommands(void) const
		{
			return mPeer.getResentReliableCommands();
		}

		/** @copydoc Peer::getLimitOfUnreliableCommands */ 
		int Client::getLimitOfUnreliableCommands(void) const
		{
			return mPeer.getLimitOfUnreliableCommands();
		}

		/** @copydoc Peer::setLimitOfUnreliableCommands */ 
		void Client::setLimitOfUnreliableCommands(int value)
		{
			mPeer.setLimitOfUnreliableCommands(value);
		}

		/** @copydoc Peer::getCRCEnabled */ 
		bool Client::getCRCEnabled(void) const
		{
			return mPeer.getCRCEnabled();
		}

		/** @copydoc Peer::setCRCEnabled */ 
		void Client::setCRCEnabled(bool crcEnabled)
		{
			mPeer.setCRCEnabled(crcEnabled);
		}

		/** @copydoc Peer::getPacketLossByCRC */
		int Client::getPacketLossByCRC(void) const
		{
			return mPeer.getPacketLossByCRC();
		}

		/** @copydoc Peer::getTrafficStatsEnabled */ 
		bool Client::getTrafficStatsEnabled(void) const
		{
			return mPeer.getTrafficStatsEnabled();
		}

		/** @copydoc Peer::setTrafficStatsEnabled */ 
		void Client::setTrafficStatsEnabled(bool trafficStatsEnabled)
		{
			mPeer.setTrafficStatsEnabled(trafficStatsEnabled);
		}

		/** @copydoc Peer::getTrafficStatsElapsedMs */ 
		int Client::getTrafficStatsElapsedMs(void) const
		{
			return mPeer.getTrafficStatsElapsedMs();
		}

		/** @copydoc Peer::getTrafficStatsIncoming */ 
		const Photon::TrafficStats& Client::getTrafficStatsIncoming(void) const
		{
			return mPeer.getTrafficStatsIncoming();
		}

		/** @copydoc Peer::getTrafficStatsOutgoing */ 
		const Photon::TrafficStats& Client::getTrafficStatsOutgoing(void) const
		{
			return mPeer.getTrafficStatsOutgoing();
		}

		/** @copydoc Peer::getTrafficStatsGameLevel */ 
		const Photon::TrafficStatsGameLevel& Client::getTrafficStatsGameLevel(void) const
		{
			return mPeer.getTrafficStatsGameLevel();
		}

		/** @copydoc Peer::getQuickResendAttempts */ 
		nByte Client::getQuickResendAttempts(void) const
		{
			return mPeer.getQuickResendAttempts();
		}

		/** @copydoc Peer::setQuickResendAttempts */ 
		void Client::setQuickResendAttempts(nByte quickResendAttempts)
		{
			mPeer.setQuickResendAttempts(quickResendAttempts);
		}

		/** @copydoc Peer::getChannelCountUserChannels() */
		nByte Client::getChannelCountUserChannels(void) const
		{
			return mPeer.getChannelCountUserChannels();
		}

		/** @copydoc Peer::getPeerCount */ 
		short Client::getPeerCount(void)
		{
			return Peer::getPeerCount();
		}
		
		/**
		   Constructor.
		   @param listener pointer to the application's implementation of the Chat:Listener callback interface.
		   @param applicationID %Photon %Chat application id
		   @param appVersion %Photon %Chat application version
		   @param connectionProtocol connection protocol*/
		Client::Client(Listener& listener, const JString& applicationID, const JString& appVersion, nByte connectionProtocol)
#if defined _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
			: mPeer(*this, connectionProtocol)
			, mListener(listener)
			, mAppVersion(appVersion)
			, mAppID(applicationID)
			, mState(ClientState::Uninitialized)
			, mAuthenticationValues(AuthenticationValues())
			, mDisconnectedCause(DisconnectCause::NONE)
			, mDidAuthenticate(false)
			, mRegion(REGION)
			, M_CONNECTION_PROTOCOL(connectionProtocol)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
		{
			mLogger.setListener(listener);

			// copy appID with '-' removed			
			mAppIDPeerInit = mAppID;
			int p = -1;
			while((p = mAppIDPeerInit.indexOf('-')) >= 0)
				mAppIDPeerInit = mAppIDPeerInit.deleteChars(p, 1);
		}
		
		/**
		   Destructor.*/
		Client::~Client(void)
		{
			for(unsigned int i=0; i<mPublicChannels.getSize(); ++i)
				ChannelFactory::destroy(mPublicChannels[i]);
			for(unsigned int i=0; i<mPrivateChannels.getSize(); ++i)
				ChannelFactory::destroy(mPrivateChannels[i]);
		}

		/** @fn template<typename Ftype> bool Client::opPublishMessage(const Common::JString& channelName, const Ftype& message)
		    Sends a message to the specified public channel.
		    @param channelName channel name
			@param message message to send
			@returns false in case of an error, true otherwise
		 */

		/** @fn template<typename Ftype> bool Client::opPublishMessage(const Common::JString& channelName, const Ftype pMessageArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize)
		    @overload
		    @param channelName channel name
		    @param pMessageArray message to send
			@param arrSize the number of elements in pParameterArray
		 */

		/** @fn template<typename Ftype> bool Client::opPublishMessage(const Common::JString& channelName, const Ftype pMessageArray, const short* pArrSizes);
			@overload
		    @param channelName channel name
		    @param pMessageArray message to send
			@param pArrSizes an array holding the number of elements for each dimension of pParameterArray
		 */

		/** @fn template<typename Ftype> bool Client::opSendPrivateMessage(const Common::JString& userName, const Ftype& message, bool encrypt)
		    Sends a private message to the specified user and creates a private channel for the conversation with that user if that channel does not exist yet.
		    @param userName user name
			@param message message to send
			@param encrypt true to send the message encrypted, false (default) to send it unencrypted
			@returns false in case of an error, true otherwise
		 */

		/** @fn template<typename Ftype> bool Client::opSendPrivateMessage(const Common::JString& userName, const Ftype pMessageArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, bool encrypt)
		    @overload
		    @param userName user name
		    @param pMessageArray message to send
			@param arrSize the number of elements in pParameterArray
			@param encrypt true to send the message encrypted, false (default) to send it unencrypted
		 */

		/** @fn template<typename Ftype> bool Client::opSendPrivateMessage(const Common::JString& userName, const Ftype pMessageArray, const short* pArrSizes, bool encrypt)
			@overload
		    @param userName user name
		    @param pMessageArray message to send
			@param pArrSizes an array holding the number of elements for each dimension of pParameterArray
			@param encrypt true to send the message encrypted, false (default) to send it unencrypted
		 */

		/** @fn template<typename Ftype> bool Client::opSetOnlineStatus(int status, const Ftype& message)
			@overload
		    @param status predefined states are in namespace UserStatus. Other values can be used at will
		    @param message optional status message
		    @returns false in case of an error, true otherwise
		 */

		/** @fn template<typename Ftype> bool Client::opSetOnlineStatus(int status, const Ftype pMessageArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize)
		    @overload
		    @param status predefined states are in namespace UserStatus. Other values can be used at will
		    @param pMessageArray optional status message
			@param arrSize the number of elements in pParameterArray
		 */

		/** @fn template<typename Ftype> bool Client::opSetOnlineStatus(int status, const Ftype pMessageArray, const short* pArrSizes)
		    @overload
		    @param status predefined states are in namespace UserStatus. Other values can be used at will
		    @param pMessageArray optional status message
			@param pArrSizes an array holding the number of elements for each dimension of pParameterArray
		 */
	
		/**
		   Initiates a connection to the %Photon name server. After a successful connection the client automatically connects to a chat front end server
		   and goes to ConnectedToFrontEnd state. After that the client can subscribe to channels and send and receive messages.
		   @param authenticationValues a user's authentication values used during connect for Custom Authentication with %Photon.
		   @param nameServerAddress used to specify a name server address different from the public %Photon Cloud name server*/
		bool Client::connect(const AuthenticationValues& authenticationValues, const JString& nameServerAddress)
		{
			JString userID;
			if(!authenticationValues.getUserID().length())
				userID = mAuthenticationValues.getUserID();
			mAuthenticationValues = authenticationValues;
			if(!authenticationValues.getUserID().length())
				mAuthenticationValues.setUserID(userID);
			mPublicChannels.removeAllElements();
			mPrivateChannels.removeAllElements();
			mDidAuthenticate = false;
#ifdef _EG_EMSCRIPTEN_PLATFORM
			const int port = M_CONNECTION_PROTOCOL==ConnectionProtocol::WS?NetworkPort::WS::NAME:NetworkPort::WSS::NAME;
			const int index = nameServerAddress.indexOf("://");
			const JString nameServerAddressWithoutProtocol = index==-1?nameServerAddress:nameServerAddress.substring(index+JString(L"://").length());
#else
			const int port = M_CONNECTION_PROTOCOL==ConnectionProtocol::UDP?NetworkPort::UDP::NAME:NetworkPort::TCP::NAME;
			const JString& nameServerAddressWithoutProtocol = nameServerAddress;
#endif
			if(mPeer.connect(nameServerAddressWithoutProtocol + (nameServerAddressWithoutProtocol.indexOf(L':') == -1?JString(L":")+(port):JString()), mAppIDPeerInit))
			{
				mListener.onStateChange(mState=ClientState::ConnectingToNameServer);
				return true;
			}
			else
				return false;
		}
		
		/**
		   Disconnects from servers.*/
		void Client::disconnect(void)
		{
			mListener.onStateChange(mState=ClientState::Disconnecting);
			mPeer.disconnect();
		}

		/** @copydoc Peer::service */ 
		void Client::service(bool dispatchIncomingCommands)
		{
			mPeer.service(dispatchIncomingCommands);
		}

		/** @copydoc Peer::serviceBasic */ 
		void Client::serviceBasic(void)
		{
			mPeer.serviceBasic();
		}

		/** @copydoc Peer::sendOutgoingCommands */ 
		bool Client::sendOutgoingCommands(void)
		{
			return mPeer.sendOutgoingCommands();
		}

		/** @copydoc Peer::sendAcksOnly */ 
		bool Client::sendAcksOnly(void)
		{
			return mPeer.sendAcksOnly();
		}

		/** @copydoc Peer::dispatchIncomingCommands */ 
		bool Client::dispatchIncomingCommands(void)
		{
			return mPeer.dispatchIncomingCommands();
		}

		/** @copydoc Peer::fetchServerTimestamp */ 
		void Client::fetchServerTimestamp(void)
		{
			mPeer.fetchServerTimestamp();
		}

		/** @copydoc Peer::resetTrafficStats */ 
		void Client::resetTrafficStats(void)
		{
			mPeer.resetTrafficStats();
		}

		/** @copydoc Peer::resetTrafficStatsMaximumCounters */
		void Client::resetTrafficStatsMaximumCounters(void)
		{
			mPeer.resetTrafficStatsMaximumCounters();
		}

		/** @copydoc Peer::vitalStatsToString */
		Common::JString Client::vitalStatsToString(bool all) const
		{
			return mPeer.vitalStatsToString(all);
		}
		
		/**
		    Sends a request to subscribe the client to the specified channels, optionally fetching messages newer than a specific ID.
		    @param channels list of channels to subscribe to.
		    @param messagesFromHistory 0: no history. 1 and higher: number of messages in history. -1: all history.
		    @returns true if request sent*/
		bool Client::opSubscribe(const JVector<JString>& channels, int messagesFromHistory)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opSubscribe: the client is in the wrong state: %d", mState);
				return false;
			}
			else
				return mPeer.opSubscribe(channels, messagesFromHistory);
		}
		
		/**
		    Unsubscribes the client from a list of channels.
		    @details
		    The client will remove these channels from the PublicChannels dictionary immediately, if it
		    could send the operation.
		    @param channels list of channels to unsubscribe from.
		    @returns true if request sent and channels removed*/
		bool Client::opUnsubscribe(const JVector<JString>& channels)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opUnsubscribe: the client is in the wrong state: %d", mState);
				return false;
			}
			else
				return mPeer.opUnsubscribe(channels);
		}

		bool Client::opPublishMessage(const JString& channelName, const Object& message)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opPublishMessage: the client is in the wrong state: %d", mState);
				return false;
			}
			else
				return mPeer.opPublishMessage(channelName, message);
		}

		bool Client::opSendPrivateMessage(const JString& userName, const Object& message, bool encrypt)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opSendPrivateMessage: the client is in the wrong state: %d", mState);
				return false;
			}
			return mPeer.opSendPrivateMessage(userName, message, encrypt);
		}

		/**
		    Sets the user's status (pre-defined or custom) and a status message.
			@details
		    The predefined status values can be found in namespace UserStatus.
		    States UserStatus::INVISIBLE and UserStatus::OFFLINE will make you offline for everyone and send no message.
		    @param status predefined states are in namespace UserStatus. Other values can be used at will
		    @returns false in case of an error, true otherwise
		 */
		bool Client::opSetOnlineStatus(int status)
		{
			return opSetOnlineStatus(status, Object(), true);
		}

		bool Client::opSetOnlineStatus(int status, const Common::Object& message, bool skipMessage)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"setOnlineStatus: the client is in the wrong state: %d", mState);
				return false;
			}
			return mPeer.opSetOnlineStatus(status, message, skipMessage);
		}
		
		/**
		    Adds users to the list on the Chat Server which will send you status updates for those.
		    @param userIDs list of friend user names
		    @returns true if the command has been sent*/
		bool Client::opAddFriends(const Common::JVector<Common::JString>& userIDs)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opAddFriends: the client is in the wrong state: %d", mState);
				return false;
			}
			return mPeer.opAddFriends(userIDs);
		}
		
		/**
		    Removes users from the list on the Chat Server which will send you status updates for those.
		    @param userIDs list of friend user names
		    @return true if the command has been sent*/
		bool Client::opRemoveFriends(const Common::JVector<Common::JString>& userIDs)
		{
			if(mState != ClientState::ConnectedToFrontEnd)
			{
				EGLOG(DebugLevel::ERRORS, L"opRemoveFriends: the client is in the wrong state: %d", mState);
				return false;
			}
			return mPeer.opRemoveFriends(userIDs);
		}
		
		/**
		    Returns the unique user id.
		    @returns the user id*/
		const JString& Client::getUserID(void) const
		{
			return mAuthenticationValues.getUserID();
		}
		
		/**
		    Returns client state*/
		int Client::getState(void) const
		{
			return mState;
		}
		
		/**
		    Returns cause of last disconnect event.
		    @returns disconnect cause constant from Chat::DisconnectCause.
		    @sa Chat::DisconnectCause*/
		int Client::getDisconnectedCause(void) const
		{
			return mDisconnectedCause;
		}
		
		/**
		    Returns chat (Name Server) region.*/
		const JString& Client::getRegion(void) const
		{
			return mRegion;
		}
		
		/**
		    Sets chat (Name Server) region.
		    Set it before connect() call.
		    @param region region*/
		void Client::setRegion(const JString& value)
		{
			mRegion = value;
		}
		
		/**
		    Returns list of subscribed public channels.
		    @returns list of subscribed channels*/
		const JVector<Channel*>& Client::getPublicChannels(void) const
		{
			return mPublicChannels;
		}
		
		/**
		    Returns list of private chats that client currently has.
		    @returns list of private chats*/
		const JVector<Channel*>& Client::getPrivateChannels(void) const
		{
			return mPrivateChannels;
		}
		
		/**
		    Search subscribed public channels by channel name.
		    @param channelName channel name to search
		    @returns found channel or NULL otherwise*/
		const Channel* Client::getPublicChannel(const JString& name) const
		{
			for(unsigned int i=0; i<mPublicChannels.getSize(); ++i)
				if(mPublicChannels[i]->getName() == name)
					return mPublicChannels[i];
			return NULL;
		}
		
		/**
		    Search private chat by user name.
		    @param userName user name to search
		    @returns found chat or NULL otherwise*/
		const Channel* Client::getPrivateChannel(const JString& userName) const
		{
			for(unsigned int i=0; i<mPrivateChannels.getSize(); ++i)
				if(mPrivateChannels[i]->getName() == userName)
					return mPrivateChannels[i];
			return NULL;
		}

		void Client::onOperationResponse(const Photon::OperationResponse& operationResponse)
		{
			EGLOG(operationResponse.getReturnCode()?DebugLevel::ERRORS:DebugLevel::INFO, operationResponse.toString(true));
			switch(operationResponse.getOperationCode())
			{
			case OperationCode::AUTHENTICATE:
				handleAuthResponse(operationResponse);
				break;
			case OperationCode::PUBLISH:
				break;
			case OperationCode::SEND_PRIVATE:
				break;
			case OperationCode::CHANNEL_HISTORY:
				break;
			case OperationCode::UPDATE_STATUS:
				break;
			case OperationCode::ADD_FRIENDS:
				break;
			case OperationCode::REMOVE_FRIENDS:
				break;
			default:
				break;
			}
		}

		void Client::onStatusChanged(int statusCode)
		{
			switch(statusCode)
			{
			case Photon::StatusCode::CONNECT:
				{
					mPeer.establishEncryption();
					if(mState == ClientState::ConnectingToNameServer)
					{
						mState = ClientState::ConnectedToNameServer;
						mListener.onStateChange(mState);
						EGLOG(DebugLevel::INFO, L"connected to nameserver");
					}
					else if(mState == ClientState::ConnectingToFrontEnd)
					{
						mPeer.opAuthenticateOnFrontEnd(mAuthenticationValues.getSecret());
						EGLOG(DebugLevel::INFO, L"connected to frontend");
					}
				}
				break;
			case Photon::StatusCode::DISCONNECT:
				{
					if(mState == ClientState::Authenticated)
					{
						mPeer.connect(mFrontEndAddress, mAppIDPeerInit);
						mListener.onStateChange(mState=ClientState::ConnectingToFrontEnd);
					}
					else
					{
						mListener.onStateChange(mState=ClientState::Disconnected);
						mListener.disconnectReturn();
					}
				}
				break;
			case Photon::StatusCode::ENCRYPTION_ESTABLISHED:
				if(!mDidAuthenticate)
				{
					if(mPeer.opAuthenticateOnNameServer(mAppID, mAppVersion, mRegion, mAuthenticationValues))
						mDidAuthenticate = true;
					else
					{
						mDidAuthenticate = false;
						mListener.debugReturn(DebugLevel::ERRORS, JString(L"Error calling OpAuthenticate! Did not work. Check log output, CustomAuthenticationValues and if you're connected. State: ") + mState);
					}
				}
				break;
			case Photon::StatusCode::ENCRYPTION_FAILED_TO_ESTABLISH:
				handleConnectionFlowError(mState, statusCode, "Encryption failed to establish");
				break;
				// cases till next break should set mDisconnectedCause below
			case Photon::StatusCode::EXCEPTION:
			case Photon::StatusCode::EXCEPTION_ON_CONNECT:
			case Photon::StatusCode::INTERNAL_RECEIVE_EXCEPTION:
			case Photon::StatusCode::TIMEOUT_DISCONNECT:
			case Photon::StatusCode::DISCONNECT_BY_SERVER:
			case Photon::StatusCode::DISCONNECT_BY_SERVER_USER_LIMIT:
			case Photon::StatusCode::DISCONNECT_BY_SERVER_LOGIC:
				mListener.connectionErrorReturn(statusCode);
				if(mPeer.getPeerState() != Photon::PeerState::DISCONNECTED && mPeer.getPeerState() != Photon::PeerState::DISCONNECTING)
					disconnect();
				break;
			case Photon::StatusCode::SEND_ERROR:
				mListener.clientErrorReturn(statusCode);
				break;
			case Photon::StatusCode::QUEUE_OUTGOING_RELIABLE_WARNING:
			case Photon::StatusCode::QUEUE_OUTGOING_UNRELIABLE_WARNING:
			case Photon::StatusCode::QUEUE_OUTGOING_ACKS_WARNING:
			case Photon::StatusCode::QUEUE_INCOMING_RELIABLE_WARNING:
			case Photon::StatusCode::QUEUE_INCOMING_UNRELIABLE_WARNING:
			case Photon::StatusCode::QUEUE_SENT_WARNING:
				mListener.warningReturn(statusCode);
				break;
			case ErrorCode::OPERATION_INVALID:
			case ErrorCode::INTERNAL_SERVER_ERROR:
				mListener.serverErrorReturn(statusCode);
				break;
			default:
				EGLOG(DebugLevel::ERRORS, L"received unknown status-code from server");
				break;
			}

			// above cases starting from StatusCode::EXCEPTION till next break should set mDisconnectedCause here
			switch(statusCode)
			{
			case Photon::StatusCode::DISCONNECT_BY_SERVER_USER_LIMIT:
				mDisconnectedCause = DisconnectCause::DISCONNECT_BY_SERVER_USER_LIMIT;
				break;
			case Photon::StatusCode::EXCEPTION_ON_CONNECT:
				mDisconnectedCause = DisconnectCause::EXCEPTION_ON_CONNECT;
				break;
			case Photon::StatusCode::DISCONNECT_BY_SERVER:
				mDisconnectedCause = DisconnectCause::DISCONNECT_BY_SERVER;
				break;
			case Photon::StatusCode::DISCONNECT_BY_SERVER_LOGIC:
				mDisconnectedCause = DisconnectCause::DISCONNECT_BY_SERVER_LOGIC;
				break;
			case Photon::StatusCode::TIMEOUT_DISCONNECT:
				mDisconnectedCause = DisconnectCause::TIMEOUT_DISCONNECT;
				break;
			case Photon::StatusCode::EXCEPTION:
			case Photon::StatusCode::INTERNAL_RECEIVE_EXCEPTION:
				mDisconnectedCause = DisconnectCause::EXCEPTION;
				break;
			default:
				break;
			}
		}

		void Client::onEvent(const Photon::EventData& eventData)
		{
			EGLOG(DebugLevel::ALL, L"%s", eventData.toString().cstr()); // don't print out the payload here, as that can get too expensive for big events
			switch(eventData.getCode())
			{
			case EventCode::CHAT_MESSAGES:
				{
					const ValueObject<JString*>& vSenders = eventData.getParameterForCode(ParameterCode::SENDERS);
					const ValueObject<Object*>& vMessages = eventData.getParameterForCode(ParameterCode::MESSAGES);

					JVector<JString> senders(*vSenders.getDataAddress(), vSenders.getSizes()[0]);
					JVector<Object> messages(*vMessages.getDataAddress(), vMessages.getSizes()[0]);

					const JString& channelName = ValueObject<JString>(eventData.getParameterForCode(ParameterCode::CHANNEL)).getDataCopy();

					Channel* pCh = getChannel(mPublicChannels, channelName);
					if(pCh)
					{
						ChannelMessageAdder::add(*pCh, senders, messages);
						mListener.onGetMessages(channelName, senders, messages);
					}
					else
						EGLOG(DebugLevel::WARNINGS, L"Got message from unsubscribed channel " + channelName);
				}
				break;
			case EventCode::PRIVATE_MESSAGE:
				{
					const Object& message = eventData.getParameterForCode(ParameterCode::MESSAGE);
					const JString& sender = ValueObject<JString>(eventData.getParameterForCode(ParameterCode::SENDER)).getDataCopy();

					JString channelName = sender;
					if(sender == getUserID())
						channelName = ValueObject<JString>(eventData.getParameterForCode(ParameterCode::USER_ID)).getDataCopy();

					ChannelMessageAdder::add(*getOrAddChannel(mPrivateChannels, channelName, 0, true), sender, message);
					mListener.onPrivateMessage(sender, message, channelName);
				}
				break;
			case EventCode::STATUS_UPDATE:
				{
					const Object& message = eventData.getParameterForCode(ParameterCode::MESSAGE);
					const JString& user = ValueObject<JString>(eventData.getParameterForCode(ParameterCode::SENDER)).getDataCopy();
					int status = ValueObject<int>(eventData.getParameterForCode(ParameterCode::STATUS)).getDataCopy();

					bool gotMessage = eventData.getParameters().contains(ParameterCode::MESSAGE);
					mListener.onStatusUpdate(user, status, gotMessage, message);
				}
				break;
			case EventCode::SUBSCRIBE:
				handleSubscribeResponse(eventData);
				break;
			case EventCode::UNSUBSCRIBE:
				handleUnsubscribeResponse(eventData);
				break;
			default:
				break;
			}
		}

		void Client::onMessage(const Common::Object& message)
		{
			EGLOG(DebugLevel::ALL, L"OnMessage: %ls", message.toString().cstr());
		}

		void Client::onRawMessage(nByte* inBuff, int inBuffBodyLength)
		{
			EGLOG(DebugLevel::ALL, L"onRawMessage, length: %d", inBuffBodyLength);

			// channel id is 4 first bytes in LE
			int channelID = *reinterpret_cast<int*>(inBuff);			

			for(unsigned int i=0; i<mPublicChannels.getSize(); ++i)
				if(channelID == mPublicChannels[i]->getID())
				{
					mListener.onReceiveBroadcastMessage(mPublicChannels[i]->getName(), inBuff+4, inBuffBodyLength-4);
					return;
				}

			EGLOG(DebugLevel::ERRORS, L"onRawMessage: can't find channel with id: %d", channelID);
		}

		void Client::debugReturn(int debugLevel, const JString& string)
		{
			mListener.debugReturn(debugLevel, string);
		}

		void Client::handleConnectionFlowError(int oldState, int errorCode, const JString& errorString)
		{
			mListener.onStateChange(mState=ClientState::Disconnecting);
			mPeer.disconnect();
			mListener.connectReturn(errorCode, errorString);
		}

		void Client::handleAuthResponse(const Photon::OperationResponse& operationResponse)
		{
			int oldState = mState;
			if(operationResponse.getReturnCode())
			{
				EGLOG(DebugLevel::ERRORS, L"Authentication failed with errorcode %d: %ls", operationResponse.getReturnCode(), operationResponse.getDebugMessage().cstr());

				switch(operationResponse.getReturnCode())
				{
				case ErrorCode::INVALID_AUTHENTICATION:
					mDisconnectedCause = DisconnectCause::INVALID_AUTHENTICATION;
					break;
				case ErrorCode::CUSTOM_AUTHENTICATION_FAILED:
					mDisconnectedCause = DisconnectCause::CUSTOM_AUTHENTICATION_FAILED;
					break;
				case ErrorCode::INVALID_REGION:
					mDisconnectedCause = DisconnectCause::INVALID_REGION;
					break;
				case ErrorCode::MAX_CCU_REACHED:
					mDisconnectedCause = DisconnectCause::MAX_CCU_REACHED;
					break;
				case ErrorCode::OPERATION_DENIED:
					mDisconnectedCause = DisconnectCause::OPERATION_NOT_ALLOWED_IN_CURRENT_STATE;
					break;
				default:
					break;
				}
				handleConnectionFlowError(oldState, operationResponse.getReturnCode(), operationResponse.getDebugMessage());
			}
			else
			{
				if(mState == ClientState::ConnectedToNameServer)
				{
					mListener.onStateChange(mState=ClientState::Authenticated);
					AuthenticationValuesSecretSetter::setSecret(mAuthenticationValues, ValueObject<JString>(operationResponse.getParameterForCode(ParameterCode::SECRET)).getDataCopy());
					mFrontEndAddress = ValueObject<JString>(operationResponse.getParameterForCode(ParameterCode::ADDRESS)).getDataCopy();
					EGLOG(DebugLevel::INFO, L"Server returned secret %ls and front end address %ls", mAuthenticationValues.getSecret().cstr(), mFrontEndAddress.cstr());
					if(operationResponse.getParameters().contains(ParameterCode::USER_ID))
					{
						mAuthenticationValues.setUserID(ValueObject<JString>(operationResponse.getParameterForCode(ParameterCode::USER_ID)).getDataCopy());
						EGLOG(DebugLevel::INFO, L"Setting userID sent by server: " + mAuthenticationValues.getUserID());
					}
					mPeer.disconnect();
				}
				else if(mState == ClientState::ConnectingToFrontEnd)
				{
					mListener.onStateChange(mState=ClientState::ConnectedToFrontEnd);
					mListener.connectReturn(0, L"");
				}
			}
		}

		void Client::handleSubscribeResponse(const Photon::EventData& eventData)
		{
			ValueObject<JString*> vChannels = eventData.getParameterForCode(ParameterCode::CHANNELS);
			ValueObject<bool*> vResults = eventData.getParameterForCode(ParameterCode::SUBSCRIBE_RESULTS);
			ValueObject<int*> vIDs = eventData.getParameterForCode(ParameterCode::CHANNEL_ID);
			ValueObject<Dictionary<Object, Object> > props = eventData.getParameterForCode(ParameterCode::PROPERTIES);
			int size = vChannels.getSizes()[0];
			JVector<JString> channels(size);
			JVector<bool> results(size);
			for(int i=0; i<size; ++i)
			{
				channels.addElement((*vChannels.getDataAddress())[i]);
				results.addElement(false);
				if((int)i < size)
				{
					bool result = (*vResults.getDataAddress())[i];
					results[i] = result;
					int id = 0;
					if(i < vIDs.getSizes()[0])
						id = (*vIDs.getDataAddress())[i];
					if(result)
					{
						Channel* ch = getOrAddChannel(mPublicChannels, channels[i], id, false);
						if(i == 0)
						{							
							Dictionary<Object, Object>* propsAddr = props.getDataAddress();
							if(propsAddr)
								ChannelPropertiesSetter::readProperties(*ch, *propsAddr);
						}
					}
				}
			}
			mListener.subscribeReturn(channels, results);
		}

		void Client::handleUnsubscribeResponse(const Photon::EventData& eventData)
		{
			ValueObject<JString*> vChannels = eventData.getParameterForCode(ParameterCode::CHANNELS);
			int size = vChannels.getSizes()[0];
			JVector<JString> channels(size);
			for(int i=0; i<size; ++i)
			{
				channels.addElement((*vChannels.getDataAddress())[i]);
				removeChannel(mPublicChannels, channels[i]);
			}
			mListener.unsubscribeReturn(channels);
		}

		Channel* Client::getChannel(JVector<Channel*>& channels, const JString& name)
		{
			for(unsigned int i=0; i<channels.getSize(); ++i)
				if(channels[i]->getName() == name)
					return channels[i];
			return NULL;
		}

		Channel* Client::getOrAddChannel(JVector<Channel*>& channels, const JString& name, int id, bool isPrivate)
		{
			Channel* pCh = getChannel(channels, name);
			if(pCh)
				return pCh;
			else
			{
				channels.addElement(ChannelFactory::create(name, id, isPrivate));
				return channels[channels.getSize()-1];
			}
		}

		void Client::removeChannel(JVector<Channel*>& channels, const JString& name)
		{
			for(unsigned int i=0; i<channels.getSize();)
			{
				if(channels[i]->getName() == name)
				{
					ChannelFactory::destroy(channels[i]);
					channels.removeElementAt(i);
				}
				else
					++i;
			}
		}
	}
}