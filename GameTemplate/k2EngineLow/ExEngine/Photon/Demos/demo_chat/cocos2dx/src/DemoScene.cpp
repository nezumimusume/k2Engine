#include "Console.h"
#include "DemoScene.h"
#include "../inc/ViewConstants.h"
#include "../../src/DemoConstants.h"

#ifdef _EG_ANDROID_PLATFORM
#include <android/log.h>
#endif

using namespace ExitGames::Common;
using namespace ExitGames::Chat;

ListController* gpLogList = NULL;

class ConsoleImplementation
{
public:
	void write(const JString& str)
	{
		gpLogList->addItem(str);
		gpLogList->scrollToBottom();
	}

	void writeLine(const JString& str)
	{
		write(str);
	}
};

Console::Console(void)
	: mpImp(new ConsoleImplementation)
{
}

Console::~Console(void)
{
	delete mpImp;
}

void Console::update(void)
{
}

void Console::write(const JString& str)
{
	mpImp->write(str);
}

void Console::writeLine(const JString& str)
{
	mpImp->writeLine(str);
}

Console& Console::get(void)
{
	static Console console;
	return console;
}

// set according to screen size in DemoScene::init()
float FONT_SIZE = 32;

// index = const in ExitGames::Chat::UserStatus
static const JString UserStatusStr[] = 
{
	L"Offline",
	L"Invisible",
	L"Online",
	L"Away",
	L"Dnd",
	L"Lfg",
	L"Playing"
};

// index = const in ExitGames::Chat::ClientState::ClientState
static const JString ClientStateStr[] = 
{
	L"Uninitialized",
	L"ConnectingToNameServer",
	L"ConnectedToNameServer",
	L"Authenticating",
	L"Authenticated",
	L"DisconnectingFromNameServer",
	L"ConnectingToFrontEnd",
	L"ConnectedToFrontEnd",
	L"Disconnecting",
	L"Disconnected"
};

DemoScene::DemoScene(void)
	: mUserID(DEFAULT_USER_ID)
	, mpLabelState(NULL)
	, mpUserIDInput(NULL)
	, mpMessageInput(NULL)
	, messageInputMaxWidth(0)
	, subscribeButtonMode(0)
	, mFriendButtonMode(0)
{
	mppSubscribeButtons[0] = NULL;
	mppSubscribeButtons[1] = NULL;
	mppFriendButtons[0] = NULL;
	mppFriendButtons[1] = NULL;
}

DemoScene::~DemoScene(void)
{
}

DemoScene* DemoScene::create(void)
{
	DemoScene *pRet = new DemoScene();
	if(pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
};

// on "init" you need to initialize your instance
bool DemoScene::init(void)
{
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(DemoScene::onTouchesBegan, this);

	auto dispatcher = this->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	float h = cocos2d::Director::getInstance()->getWinSize().height;
	FONT_SIZE = h * 5 / 100;

	return true;
}

void DemoScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
}

void DemoScene::resetInputFocus(void)
{
	mpUserIDInput->detachWithIME();
	mpMessageInput->detachWithIME();
}

static void drawBorder(cocos2d::Node* node)
{
	cocos2d::DrawNode* d = cocos2d::DrawNode::create();
	float width = node->getContentSize().width;
	float height = node->getContentSize().height;
	float marginX = cocos2d::Director::getInstance()->getWinSize().width / 100 + 2;
	float marginY = - height / 10 + 2; // negative for text labels
	cocos2d::Point pp[] = { cocos2d::Point(-marginX, -marginY), cocos2d::Point(width + marginX, -marginY), cocos2d::Point(width + marginX, height + marginY), cocos2d::Point(-marginX, height + marginY)};
	d->drawPolygon(pp, 4, BUTTON_COLOR, 1, BUTTON_BORDER_COLOR);
	d->setAnchorPoint(node->getAnchorPoint());
	node->addChild(d);
}

void DemoScene::setupUI(void)
{
	cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
	float y = sz.height;
	float spacing = sz.width / 10;
	cocos2d::Color3B grey = {180,180,180};

	cocos2d::Label* header = cocos2d::Label::createWithTTF("Photon Chat Demo", FONT_NAME, FONT_SIZE * 0.7f);
	header->setColor(grey);
	header->setAnchorPoint(cocos2d::Point(0, 1));
	header->setPosition(cocos2d::Point(0, y));
	addChild(header);

	y -= header->getContentSize().height;

	mpLabelState = cocos2d::Label::createWithTTF("Undef", FONT_NAME, FONT_SIZE * 0.7f);
	mpLabelState->setColor(grey);
	mpLabelState->setAnchorPoint(cocos2d::Point(0, 1));
	mpLabelState->setPosition(cocos2d::Point(0, y));
	addChild(mpLabelState);

	y -= mpLabelState->getContentSize().height;

	cocos2d::Label* userIDLabel = cocos2d::Label::createWithTTF("User id: ", FONT_NAME, FONT_SIZE);
	userIDLabel->setAnchorPoint(cocos2d::Point(0, 1));
	userIDLabel->setPosition(cocos2d::Point(0, y));

	mpUserIDInput = cocos2d::TextFieldTTF::textFieldWithPlaceHolder("<set user id>", FONT_NAME, FONT_SIZE);
	mpUserIDInput->setDelegate(this);
	mpUserIDInput->setString(DEFAULT_USER_ID.ANSIRepresentation().cstr());
	mpUserIDInput->setColor(INPUT_COLOR);
	mpUserIDInput->setContentSize(cocos2d::Size(sz.width - userIDLabel->getContentSize().width, mpUserIDInput->getContentSize().height));
	cocos2d::MenuItemLabel* userIDInputButton = cocos2d::MenuItemLabel::create(mpUserIDInput, CC_CALLBACK_1(DemoScene::menuUserIDTouchCallback, this));
	userIDInputButton->setAnchorPoint(cocos2d::Point(1, 1));
	cocos2d::Menu* userIDInputMenu = cocos2d::Menu::create(userIDInputButton, NULL);
	userIDInputMenu->setPosition(sz.width, y);

	addChild(userIDLabel);
	addChild(userIDInputMenu);

	y -= mpUserIDInput->getContentSize().height;

	cocos2d::MenuItem* b0 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Connect", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuConnectCallback, this));
	cocos2d::MenuItem* b1 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Disconnect", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuDisconnectCallback, this));
	cocos2d::MenuItem* b20 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Subscribe", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuSubscribeCallback, this));
	cocos2d::MenuItem* b21 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Unsubscr.", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuUnsubscribeCallback, this));
	cocos2d::MenuItem* b30 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Set friends", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuSetFriendsCallback, this));
	cocos2d::MenuItem* b31 = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Clr. friends", FONT_NAME, FONT_SIZE), CC_CALLBACK_1(DemoScene::menuClearFriendsCallback, this));
	drawBorder(b0);
	drawBorder(b1);
	drawBorder(b20);
	drawBorder(b21);
	drawBorder(b30);
	drawBorder(b31);

	mppSubscribeButtons[0] = b20;
	mppSubscribeButtons[1] = b21;
	mppFriendButtons[0] = b30;
	mppFriendButtons[1] = b31;

	b0->setAnchorPoint(cocos2d::Point(0, 1));
	b1->setAnchorPoint(cocos2d::Point(1, 1));
	b20->setAnchorPoint(cocos2d::Point(0, 1));
	b21->setAnchorPoint(cocos2d::Point(0, 1));
	b30->setAnchorPoint(cocos2d::Point(1, 1));
	b31->setAnchorPoint(cocos2d::Point(1, 1));
	int bh = (int)b0->getContentSize().height;
	b0->setPosition(cocos2d::Point(0, 0));
	b1->setPosition(cocos2d::Point(sz.width, 0));
	b20->setPosition(cocos2d::Point(0, -(float)bh));
	b21->setPosition(cocos2d::Point(0, -(float)bh));
	b30->setPosition(cocos2d::Point(sz.width, -(float)bh));
	b31->setPosition(cocos2d::Point(sz.width, -(float)bh));

	cocos2d::Menu* pMenu = cocos2d::Menu::create(b0, b1, b20, b21, b30, b31, NULL);
	pMenu->setPosition(0, y);
	addChild(pMenu);

	y -= bh*2;
	
	const float logHeight = sz.height/3;
	gpLogList = ListController::create(sz.width, logHeight, this, (ListControllerHandler)&DemoScene::logTouchCallback);
	gpLogList->setAnchorPoint(cocos2d::Point(0, 1));
	gpLogList->setPosition(cocos2d::Point(0, y));
	addChild(gpLogList);

	y -= logHeight;

	y -= FONT_SIZE/2;

	cocos2d::MenuItem* sendButton = cocos2d::MenuItemLabel::create(cocos2d::Label::createWithTTF("Send", FONT_NAME, FONT_SIZE * 1.2f), CC_CALLBACK_1(DemoScene::menuSendCallback, this));
	drawBorder(sendButton);
	sendButton->setAnchorPoint(cocos2d::Point(1, 1));
	sendButton->setPosition(cocos2d::Point(sz.width, 0));
	cocos2d::Menu* sendMenu = cocos2d::Menu::create(sendButton, NULL);
	sendMenu->setPosition(0, y);
	addChild(sendMenu);

	messageInputMaxWidth = sz.width - sendButton->getContentSize().width - spacing;
	mpMessageInput = cocos2d::TextFieldTTF::textFieldWithPlaceHolder("<message>", cocos2d::Size(0, 0), cocos2d::TextHAlignment::LEFT, FONT_NAME, FONT_SIZE * 1.2f);
	mpMessageInput->setDelegate(this);
	mpMessageInput->setColor(INPUT_COLOR);
	cocos2d::MenuItemLabel* messageInputButton = cocos2d::MenuItemLabel::create(mpMessageInput, CC_CALLBACK_1(DemoScene::menuMessageTouchCallback, this));
	messageInputButton->setAnchorPoint(cocos2d::Point(0, 1));
	cocos2d::Menu* messageInputMenu = cocos2d::Menu::create(messageInputButton, NULL);
	messageInputMenu->setPosition(0, y);
	addChild(messageInputMenu);

	y -= mpMessageInput->getContentSize().height;

	y -= FONT_SIZE/2;

	cocos2d::Menu* userStatusMenu = cocos2d::Menu::create();
	userStatusMenu->setPosition(0, 0);
	float x = 0;
	for(int i=0; i<sizeof(UserStatusStr)/sizeof(JString); ++i)
	{
		cocos2d::Label* l = cocos2d::Label::createWithTTF(UserStatusStr[i].UTF8Representation().cstr(), FONT_NAME, FONT_SIZE);
		drawBorder(l);
		cocos2d::MenuItem* b = cocos2d::MenuItemLabel::create(l, CC_CALLBACK_1(DemoScene::menuUserStatusCallback, this));
		b->setTag(i);
		b->setAnchorPoint(cocos2d::Point(0, 1));
		cocos2d::Size size = b->getContentSize();
		if(x + size.width > sz.width)
		{
			x = 0;
			y -= size.height;
		}
		b->setPosition(cocos2d::Point(x, y));
		x += size.width + spacing;
		userStatusMenu->addChild(b);
	}
	addChild(userStatusMenu);

	subscribeButtonModeSet(0);
	friendButtonModeSet(0);
}

void DemoScene::subscribeButtonModeSet(int m)
{
	subscribeButtonMode = m;
	if(m >= 0 && m < 2)
	{
		mppSubscribeButtons[m]->setVisible(true);
		mppSubscribeButtons[1 - m]->setVisible(false);
	}
}

void DemoScene::friendButtonModeSet(int m)
{
	mFriendButtonMode = m;
	if(m >= 0 && m < 2)
	{
		mppFriendButtons[m]->setVisible(true);
		mppFriendButtons[1 - m]->setVisible(false);
	}
}

void DemoScene::messageInputScroll(void)
{
	float pos = messageInputMaxWidth - mpMessageInput->getContentSize().width;
	if(pos < 0) 
		mpMessageInput->setPositionX(pos);
	else
		mpMessageInput->setPositionX(0);
}

bool DemoScene::onTextFieldInsertText(cocos2d::TextFieldTTF* sender, const char* text, size_t nLen)
{
	if(sender == mpMessageInput)
		messageInputScroll();
	if(sender == mpMessageInput && '\n' == *text) 
		sendMessage();
	return false;
}

bool DemoScene::onTextFieldDeleteBackward(cocos2d::TextFieldTTF* sender, const char* delText, size_t nLen)
{
	if(sender == mpMessageInput)
		messageInputScroll();
	return false;
}

bool DemoScene::onTextFieldAttachWithIME(cocos2d::TextFieldTTF* sender)
{
#ifndef _EG_WINDOWS_PLATFORM
#ifndef _EG_MARMALADE_PLATFORM
	float y = sender->convertToWorldSpace(cocos2d::Point(0,0)).y;
	float y0 = cocos2d::Director::getInstance()->getWinSize().height / 2;
	if(y < y0)
		setPositionY(y0 - y);
#endif
#endif
	return false;
}

bool DemoScene::onTextFieldDetachWithIME(cocos2d::TextFieldTTF* sender)
{
#ifndef _EG_WINDOWS_PLATFORM
#ifndef _EG_MARMALADE_PLATFORM
	setPositionY(0);
#endif
#endif
	return false;
}

void DemoScene::setClient(Client* c)
{
	this->mpClient = c;
}

void DemoScene::menuUserIDTouchCallback(Ref* pSender)
{
	mpUserIDInput->attachWithIME();
}

void DemoScene::menuMessageTouchCallback(Ref* pSender)
{
	mpMessageInput->attachWithIME();
}

void DemoScene::menuConnectCallback(Ref* pSender)
{
	resetInputFocus();

	Console::get().writeLine(L"[i]: Connecting...");
	mUserID = mpUserIDInput->getString().c_str();
	mpClient->connect(AuthenticationValues().setUserID(mUserID));
}

void DemoScene::menuDisconnectCallback(Ref* pSender)
{
	resetInputFocus();

	if(mpClient->getState() == ClientState::Uninitialized || mpClient->getState() == ClientState::Disconnected)
		cocos2d::Director::getInstance()->popScene();
	else
		mpClient->disconnect();
}

void DemoScene::menuSubscribeCallback(Ref* pSender)
{
	resetInputFocus();

	int size = sizeof(SUBSCRIBE_CHANNELS)/sizeof(JString);
	JVector<JString> chs(size);
	for(int i=0; i<size; ++i)
		chs.addElement(SUBSCRIBE_CHANNELS[i]);
	if(mpClient->opSubscribe(chs))
		Console::get().writeLine(L"[i]: Subscribing...");
	else
		Console::get().writeLine(L"[i]: Connect first");
}

void DemoScene::menuSetFriendsCallback(Ref* pSender)
{
	resetInputFocus();

	JVector<JString> friends(USER_COUNT);
	for(int i=0; i<USER_COUNT; ++i)
		friends.addElement(USER_PREFIX+i);
	if(mpClient->opAddFriends(friends))
	{
		Console::get().writeLine(L"[i]: Setting friends: " + friends.toString() + L" ...");
		friendButtonModeSet(1);
	}
	else
		Console::get().writeLine(L"[i]: Connect first");
}

void DemoScene::menuClearFriendsCallback(Ref* pSender)
{
	resetInputFocus();

	JVector<JString> friends(USER_COUNT);
	for(int i=0; i<USER_COUNT; ++i)
		friends.addElement(USER_PREFIX+i);
	if(mpClient->opRemoveFriends(friends))
		Console::get().writeLine(L"[i]: Clearing friends...");
	else
		Console::get().writeLine(L"[i]: Connect first");
	friendButtonModeSet(0);
}

void DemoScene::menuUnsubscribeCallback(Ref* pSender)
{
	resetInputFocus();

	int size = sizeof(SUBSCRIBE_CHANNELS)/sizeof(JString);
	JVector<JString> chs(size);
	for(int i=0; i<size; ++i)
		chs.addElement(SUBSCRIBE_CHANNELS[i]);
	if(mpClient->opUnsubscribe(chs))
		Console::get().writeLine(L"[i]: Unsubscribing...");
	else
		Console::get().writeLine(L"[i]: Connect first");
}

void DemoScene::menuSendCallback(Ref* pSender)
{
	sendMessage();
}

void DemoScene::menuUserStatusCallback(Ref* pSender)
{
	resetInputFocus();

	cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)pSender;
	if(mpClient->opSetOnlineStatus(item->getTag(), L"My status changed")) // update message
		Console::get().writeLine(JString(L"[i] my status sent: ") + item->getTag());
}

void DemoScene::sendMessage(void) 
{
	JString s = mpMessageInput->getString().c_str();
	int index;
	JString sep = s.substring((index=s.indexOf(L':'))<0?index:index+1);
	if(sep.length()) // publish
		mpClient->opPublishMessage(s.substring(0, index), sep);
	else if((sep=s.substring((index=s.indexOf(L'@'))<0?index:index+1)).length()) // private
			mpClient->opSendPrivateMessage(s.substring(0, index), sep);
	else // publish to random channel
	{
		static int cnt = 0;
		const JVector<Channel*>& chs = mpClient->getPublicChannels();
		if(chs.getSize())
			mpClient->opPublishMessage(chs[cnt++ % chs.getSize()]->getName(), s);
		else
			Console::get().writeLine(L"[i]: Not subscribed to any channels");
	}
	mpMessageInput->setString(std::string());
	messageInputScroll();
}

void DemoScene::logTouchCallback(const JString& name)
{
}

void DemoScene::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void DemoScene::onStateChange(int state)
{
	mpLabelState->setString((JString()+(state >=0 && state < sizeof(ClientStateStr)/sizeof(JString) ? ClientStateStr[state] : "?")).UTF8Representation().cstr());
	if(state == ClientState::ConnectedToFrontEnd)
	{
		Console::get().writeLine(L"[i]: [Subscribe] for public channels");
		Console::get().writeLine(L"[i]:    or type in 'userid@message'");
		Console::get().writeLine(L"[i]:    and press 'OK' for private");
		subscribeButtonModeSet(0);
	}
	else if(state == ClientState::Uninitialized || state == ClientState::Disconnected)
	{
		if(mpClient->getDisconnectedCause() == DisconnectCause::INVALID_AUTHENTICATION)
		{
			Console::get().writeLine(L"[i]: Disconnected due to invalid authentication");
			Console::get().writeLine(L"[i]: Is app id correct?");
		}
		else
			Console::get().writeLine(L"[i]: Disconnected");
		Console::get().writeLine(L"-------------------------------------------------");
		Console::get().writeLine(L"[i]: touch log lines to see details");
		Console::get().writeLine(L"[i]: type in user id and press [Connect]");
		subscribeButtonModeSet(0);
		friendButtonModeSet(0);
	}
}

void DemoScene::connectionErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"connection failed with error %d", errorCode);
	Console::get().writeLine(JString(L"[i]: Connection Error ") + errorCode);
}

void DemoScene::clientErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"received error %d from client", errorCode);
	Console::get().writeLine(JString(L"[i]: Error ") + errorCode);
}

void DemoScene::warningReturn(int warningCode)
{
	EGLOG(DebugLevel::WARNINGS, L"received warning %d from client", warningCode);
	Console::get().writeLine(JString(L"[i]: Warning ") + warningCode);
}

void DemoScene::serverErrorReturn(int errorCode)
{
	EGLOG(DebugLevel::ERRORS, L"received error %d from server", errorCode);
	Console::get().writeLine(JString(L"[i]: Server Error ") + errorCode);
}

void DemoScene::connectReturn(int errorCode, const JString& errorString)
{
	Console::get().writeLine(L"[i]: Connected to Front End");
}

void DemoScene::disconnectReturn(void)
{	
}

void DemoScene::subscribeReturn(const JVector<JString>& channels, const JVector<bool>& results)
{
	JVector<JString> ok(100);
	for(unsigned int i=0; i<channels.getSize(); ++i) 
		if(results[i])
			ok.addElement(channels[i]);
	Console::get().writeLine(L"[i]: Subscribed to " + ok.toString());
	Console::get().writeLine(L"[i]: type in 'channel:message'");
	Console::get().writeLine(L"[i]:   and press [Send] to publish");
	subscribeButtonModeSet(1);
}

void DemoScene::unsubscribeReturn(const JVector<JString>& channels)
{
	Console::get().writeLine(L"[i]: Unsubscribed from " + channels.toString());
	subscribeButtonModeSet(0);
}

void DemoScene::onGetMessages(const JString& channelName, const JVector<JString>& senders, const JVector<Object>& messages)
{
	for(unsigned int i=0; i<senders.getSize(); ++i) 
		Console::get().writeLine(L"["+ channelName + L":"+ senders[i] + L"]: " + messages[i].toString());
}

void DemoScene::onPrivateMessage(const JString& sender, const Object& message, const JString& channelName)
{
	Console::get().writeLine(L"["+ channelName + L"@"+ sender + L"]: " + message.toString());
}

void DemoScene::onStatusUpdate(const JString& user, int status, bool gotMessage, const Object& message)
{
	const JString statusStr = (status < sizeof(UserStatusStr)/sizeof(JString)) ?  UserStatusStr[status] : L"";
	Console::get().writeLine(L"[i]: " + user + L": " + statusStr + L"(" + status + L") / " + (gotMessage?message.toString():L"[message skipped]"));
}
