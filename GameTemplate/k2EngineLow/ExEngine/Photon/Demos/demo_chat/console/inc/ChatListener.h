#pragma once

#include "Console.h"
#include "Chat-cpp/inc/Client.h"

class ChatListener : public ExitGames::Chat::Listener
{
public:
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

	virtual void onStateChange(int state);

	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);

	virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
	virtual void disconnectReturn(void);
	virtual void subscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels, const ExitGames::Common::JVector<bool>& results);
	virtual void unsubscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels);

	virtual void onGetMessages(const ExitGames::Common::JString& channelName, const ExitGames::Common::JVector<ExitGames::Common::JString>& senders, const ExitGames::Common::JVector<ExitGames::Common::Object>& messages);
	virtual void onPrivateMessage(const ExitGames::Common::JString& sender, const ExitGames::Common::Object& message, const ExitGames::Common::JString& channelName);
	virtual void onStatusUpdate(const ExitGames::Common::JString& user, int status, bool gotMessage, const ExitGames::Common::Object& message);

	virtual void onReceiveBroadcastMessage(const ExitGames::Common::JString& channelName, nByte* inBuff, int inBuffBodyLength);
};