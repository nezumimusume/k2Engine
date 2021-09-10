#pragma once

#include "cocos2d.h"

#include "Chat-cpp/inc/Client.h"
#include "Chat-cpp/inc/Listener.h"
#include "ListController.h"

#include <vector>

class DemoScene : public cocos2d::Scene, public cocos2d::TextFieldDelegate, public ExitGames::Chat::Listener
{
public:
	DemoScene(void);
	~DemoScene(void);
	static DemoScene* create(void);
	
	void setClient(ExitGames::Chat::Client* client);
	void setupUI(void);

	// ExitGames::Chat::Listener
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

	// cocos2d::CCLayer
	virtual bool init(void);
	
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	// cocos2d::CCTextFieldDelegate
	virtual bool onTextFieldInsertText(cocos2d::TextFieldTTF* pSender, const char* text, size_t nLen);
	virtual bool onTextFieldDeleteBackward(cocos2d::TextFieldTTF* sender, const char* delText, size_t nLen);
	// Move scene to avoid text field overlap with keyboard. Should use CCIMEDelegate::keyboardWillShow instead but it does not work properly.
	virtual bool onTextFieldAttachWithIME(cocos2d::TextFieldTTF* sender);
    virtual bool onTextFieldDetachWithIME(cocos2d::TextFieldTTF* sender);

private:
	ExitGames::Common::Logger mLogger;
	ExitGames::Chat::Client* mpClient;
	ExitGames::Common::JString mUserID;

	cocos2d::Label* mpLabelState;
	cocos2d::TextFieldTTF* mpUserIDInput;
	cocos2d::TextFieldTTF* mpMessageInput;
	// scroll support
	float messageInputMaxWidth;
	void messageInputScroll(void);

	// subscribe to channels or friends
	cocos2d::MenuItem* mppSubscribeButtons[2];
	cocos2d::MenuItem* mppFriendButtons[2];
	int subscribeButtonMode;
	void subscribeButtonModeSet(int);
	int mFriendButtonMode;
	void friendButtonModeSet(int);

	// button made of user id inout - attaches keyboard on press
	void menuUserIDTouchCallback(Ref* pSender);
	// button made of message inout - attaches keyboard on press
	void menuMessageTouchCallback(Ref* pSender);
	// other buttons
	void menuConnectCallback(Ref* pSender);
	void menuDisconnectCallback(Ref* pSender);
	void menuSubscribeCallback(Ref* pSender);
	void menuSetFriendsCallback(Ref* pSender);
	void menuClearFriendsCallback(Ref* pSender);
	void menuUnsubscribeCallback(Ref* pSender);
	void menuSendCallback(Ref* pSender);
	// user status change buttons
	void menuUserStatusCallback(Ref* pSender);

	void onTouch(cocos2d::Touch* touch, cocos2d::Event* event);

	// buttons mask the scene and prevent keyboard detach from input when touch is out of input, need to call this on every button press
	void resetInputFocus(void);

	// empty (not used)
	void logTouchCallback(const ExitGames::Common::JString& name);

	void sendMessage(void);
};