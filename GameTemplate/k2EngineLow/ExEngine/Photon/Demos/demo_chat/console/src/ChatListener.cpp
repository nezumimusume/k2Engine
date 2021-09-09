#include "ChatListener.h"

using namespace ExitGames;

void ChatListener::debugReturn(int debugLevel, const Common::JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void ChatListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString(L"connectionErrorReturn: ") + errorCode);
}

void ChatListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString(L"clientErrorReturn: ") + errorCode);
}

void ChatListener::warningReturn(int warningCode)
{
	Console::get().writeLine(Common::JString(L"warningReturn: ") + warningCode);
}

void ChatListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(Common::JString(L"serverErrorReturn: ") + errorCode);
}

void ChatListener::connectReturn(int errorCode, const Common::JString& errorString)
{
	Console::get().writeLine(Common::JString(L"connectReturn: ") + errorCode + (errorString.length()?(", " + errorString):L""));
}

void ChatListener::disconnectReturn(void)
{
	Console::get().writeLine(L"disconnectReturn");
}

void ChatListener::onStateChange(int state)
{
	Console::get().writeLine(Common::JString(L"onStateChange: ") + state);
}

void ChatListener::onGetMessages(const Common::JString& channelName, const Common::JVector<Common::JString>& senders, const Common::JVector<Common::Object>& messages)
{
	for(unsigned int i=0; i<senders.getSize(); ++i)
		Console::get().writeLine(L"[" + channelName + L": " + senders[i] + L"] >>> " + messages[i].toString());
}

void ChatListener::onPrivateMessage(const Common::JString& sender, const Common::Object& message, const Common::JString& channelName)
{
	Console::get().writeLine(L"[Private '" + channelName + L"': " + sender + L"] >>> " + message.toString());
}

void ChatListener::subscribeReturn(const Common::JVector<Common::JString>& channels, const Common::JVector<bool>& results)
{
	Console::get().writeLine(L"subscribeReturn: "+  channels.toString() + L", " + results.toString());
}

void ChatListener::unsubscribeReturn(const Common::JVector<Common::JString>& channels)
{
	Console::get().writeLine(L"unsubscribeReturn: " + channels.toString());
}

void ChatListener::onStatusUpdate(const Common::JString& user, int status, bool gotMessage, const Common::Object& message)
{
	Console::get().writeLine(L"onStatusUpdate: " + user + L" has changed his/her status to: " + status + L" / " + (gotMessage?message.toString():L"[message skipped]"));
}

void ChatListener::onReceiveBroadcastMessage(const Common::JString& channelName, nByte* inBuff, int inBuffBodyLength)
{
	Console::get().writeLine(L"onReceiveBroadcastMessage: " + channelName + L": " + inBuffBodyLength + L" bytes");
}
