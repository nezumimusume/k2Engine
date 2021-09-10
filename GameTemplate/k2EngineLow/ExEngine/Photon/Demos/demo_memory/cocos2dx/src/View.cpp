#include "View.h"
#include "ViewConstants.h"
#include "../../src/DemoConstants.h"
#include "LoadBalancing-cpp/inc/Enums/PeerStates.h"
#include <math.h>

using namespace ExitGames::Common;

#define Z_TOPMOST   100

const char* tileIconFiles[] =
{
"tile-angler-fish.png",
"tile-crown.png",
"tile-drink-me.png",
"tile-fishbone.png",
"tile-glass-heart.png",
"tile-horse-head.png",
"tile-robe.png",
"tile-rocket.png",
};

const char* FONT_MEDIUM = "arial28.fnt";
const char* FONT_SMALL = "arial20.fnt";

// setContentSize does not work as expected, returning size by parameter
static cocos2d::MenuItem* decorateButton(cocos2d::MenuItemLabel* item, cocos2d::Size& size)
{
	const cocos2d::Color4F BORDER_COLOR = {1, 1, 1, 0.5};
	const cocos2d::Color4F NORMAL_COLOR = {1, 1, 1, 0.3f};

	cocos2d::DrawNode* bg = cocos2d::DrawNode::create();
	bg->setPosition(0, 0);
	float lw = item->getLabel()->getContentSize().width;
	float lh = item->getLabel()->getContentSize().height;
	float marginX = lh*0.2f;
	float marginY = lh*0.05f;
	float w = lw + 2*marginX;
	float h = lh + 2*marginY;
	cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(w, 0), cocos2d::Point(w, h), cocos2d::Point(0, h)};
	bg->drawPolygon(pp, 4, NORMAL_COLOR, 1, BORDER_COLOR);
	bg->setPosition(-marginX, - marginY);
	item->addChild(bg);
	// adding another margin around rect
	size.width = w + marginX;
	size.height = h + marginY;
	return item;
}

static cocos2d::MenuItem* createButton(cocos2d::Label* label, const cocos2d::ccMenuCallback& callback, cocos2d::Size& size)
{
	cocos2d::MenuItemLabel* item = cocos2d::MenuItemLabel::create(label, callback);
	decorateButton(item, size);
	return item;
}

View::View(void)
	: mpLbl(NULL)
	, mBoardSize(0)
	, mTileSize(0)
	, mTileMargin(0)
	, mTileRows(0)
	, mTileCols(0)
	, mpSceneConnect(NULL)
	, mpSceneLobby(NULL)
	, mpSceneGame(NULL)
	, mpCurrentScene(NULL)
	, mpBoard(NULL)
	, mpTiles(NULL) 
	, mpLabelState(NULL)
	, mpLabelLog(NULL)
	, mpRoomList(NULL)
	, mpSavedRoomList(NULL)
	, mpLobbyStats1(NULL)
	, mpLobbyStats2(NULL)
	, mpLabelMessage(NULL)
	, mStateString(JString())
	, mpInputUserName(NULL)
{
}

View::~View(void)
{
}

View* View::create(void)
{
	View* pRet = new View();
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

void View::init(LoadBalancingListener* lbl) 
{
	this->mpLbl = lbl;
}

// on "init" you need to initialize your instance
bool View::init(void)
{
	if (!Layer::init())
	{
		return false;
	}

	auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(View::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(View::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(View::onTouchesEnded, this);

	auto dispatcher = this->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void View::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{

	cocos2d::Point touchLocation = touches[0]->getLocation();
	cocos2d::Point pos = mpBoard->convertToNodeSpace(touchLocation);
//	return pos.x > 0 && pos.x < mBoardSize && pos.y > -mBoardSize && pos.y < 0;
}

void View::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
}

void View::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
	cocos2d::Point touchLocation = touches[0]->getLocation();
	cocos2d::Point pos = mpBoard->convertToNodeSpace(touchLocation);
	int x = (int)(pos.x / (mTileSize + 2 * mTileMargin));
	int y = (int)((mBoardSize + pos.y) / (mTileSize + 2 * mTileMargin));
	if(x >= 0 && x < mTileCols && y >= 0 && y < mTileRows)
		mpLbl->click(x + y * mTileCols);
}

void View::setup(void)
{
    setupUI();
    EGLOG(DebugLevel::INFO, L"setup");
    switchScene(mpSceneConnect);
}

void View::setupUI(void)
{
	cocos2d::MenuItem *b0, *b1, *b2, *b3;
	cocos2d::Menu* menu;
	float y;

	cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
	mBoardSize = sz.width < sz.height ? sz.width : sz.height;
	mTileMargin = ceilf(sz.height / 200);

	// common controls
		
	mpLabelState = cocos2d::Label::createWithBMFont(FONT_SMALL, "Undef", cocos2d::TextHAlignment::LEFT, (int)mBoardSize);
	mpLabelState->setLineBreakWithoutSpace(true);
	mpLabelLog = cocos2d::Label::createWithBMFont(FONT_SMALL, "Undef", cocos2d::TextHAlignment::LEFT, (int)mBoardSize);
	addChild(mpLabelState);
	addChild(mpLabelLog);
	mpLabelState->setAnchorPoint(cocos2d::Point(0, 1));
	mpLabelLog->setAnchorPoint(cocos2d::Point(0, 1));
	mpLabelState->setPosition(0, sz.height);                                 // state docked to screen top
	mpLabelLog->setPosition(0, mpLabelLog->getContentSize().height * 4);	   // log docked to screen bottom

	// scenes
	float y0 = sz.height - mpLabelState->getContentSize().height;
	mpSceneConnect = Node::create();
	mpSceneLobby = Node::create();
	mpSceneGame = Node::create();
	addChild(mpSceneConnect);
	addChild(mpSceneLobby);
	addChild(mpSceneGame);
	mpSceneConnect->setPositionY(y0);
	mpSceneLobby->setPositionY(y0);
	mpSceneGame->setPositionY(y0);

	// mpSceneConnect
	y = - sz.height / 3;
	cocos2d::Label* l = cocos2d::Label::createWithBMFont(FONT_MEDIUM, "User:");
	float lw = l->getContentSize().width;
	float lh = l->getContentSize().height;
	float marginX = lh*0.3f;
	float marginY = lh*0.15f;
	l->setAnchorPoint(cocos2d::Point(0, 0.5f));
	l->setPosition(marginX, y);
	mpSceneConnect->addChild(l);

	mpInputUserName = cocos2d::TextFieldTTF::textFieldWithPlaceHolder("<set user name>", "", 28);
	mpInputUserName->setDelegate(this);
	mpInputUserName->setString(DEFAULT_USER_NAME.ANSIRepresentation().cstr());
	mpInputUserName->setColor(INPUT_COLOR);
	cocos2d::MenuItemLabel* inputUserNameButton = cocos2d::MenuItemLabel::create(mpInputUserName, CC_CALLBACK_1(View::menuInputUserNameTouchCallback, this));
	inputUserNameButton->setAnchorPoint(cocos2d::Point(0, 0.5f));
	inputUserNameButton->setPosition(cocos2d::Point(marginX + lw + marginX, y));
	cocos2d::Menu* inputUserNameMenu = cocos2d::Menu::create(inputUserNameButton, NULL);
	inputUserNameMenu->setAnchorPoint(cocos2d::Point(0, 0));
	inputUserNameMenu->setPosition(0,0);
	mpSceneConnect->addChild(inputUserNameMenu);
	y -= 2*lh;

	cocos2d::Size bs;
	b0 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Connect"), CC_CALLBACK_1(View::menuConnectCallback, this), bs);
	menu = cocos2d::Menu::create(b0, NULL);
	menu->setPosition(0, 0);
	b0->setAnchorPoint(cocos2d::Point(0.5f, 0.5f));
	b0->setPosition(mBoardSize/2, y);  y -= bs.height;
	mpSceneConnect->addChild(menu);

	// mpSceneLobby
	y = 0;
	float roomListHeight = sz.height / 4;
	mpLobbyStats1 = cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Saved Games:");
	mpSavedRoomList = ListController::create(sz.width, roomListHeight, this, (ListControllerHandler)&View::savedRoomListCallback);
	b0 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Refresh"), CC_CALLBACK_1(View::menuRefreshCallback, this), bs);
	b1 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Leave"),  CC_CALLBACK_1(View::menuLeaveCallback, this), bs);
	mpLobbyStats2 = cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Rooms in lobby:");
	mpRoomList = ListController::create(sz.width, roomListHeight, this, (ListControllerHandler)&View::roomListCallback);
	b2 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Random"), CC_CALLBACK_1(View::menuRandomGameCallback, this), bs);
	b3 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "New Game"),CC_CALLBACK_1(View::menuNewGameCallback, this), bs);
	mpLobbyStats1->setAnchorPoint(cocos2d::Point(0, 1));
	mpLobbyStats2->setAnchorPoint(cocos2d::Point(0, 1));
	b0->setAnchorPoint(cocos2d::Point(0, 0.5f));
	b1->setAnchorPoint(cocos2d::Point(1, 0.5f));
	b2->setAnchorPoint(cocos2d::Point(0, 0.5f));
	b3->setAnchorPoint(cocos2d::Point(1, 0.5f));
	float bh = bs.height;
	float bsh2 = bs.height/2;
	mpLobbyStats1->setPositionY(y);                                     y -= mpLobbyStats1->getContentSize().height;
	mpSavedRoomList->setPosition(0, y);                                 y -= roomListHeight + marginY;
	b0->setPosition(marginX, y - bsh2);  b1->setPosition(mBoardSize - marginX, y - bsh2);  y -= bh;
	mpLobbyStats2->setPositionY(y);                                     y -= mpLobbyStats2->getContentSize().height;
	mpRoomList->setPosition(0, y);                                      y -= roomListHeight + marginY;
	b2->setPosition(marginX, y - bsh2);  b3->setPosition(mBoardSize- marginX, y - bsh2);  y -= bh;
	
	menu = cocos2d::Menu::create(b0, b1, b2, b3, NULL);
	menu->setPosition(0, 0);
	
	mpSceneLobby->addChild(mpLobbyStats1);
	mpSceneLobby->addChild(mpSavedRoomList);	
	mpSceneLobby->addChild(mpLobbyStats2);
	mpSceneLobby->addChild(mpRoomList);
	mpSceneLobby->addChild(menu);
	
	// mpSceneGame
	y = 0;
	mpLabelMessage = cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Welcome");
	mpLabelMessage->setAnchorPoint(cocos2d::Point(0, 1));
	mpLabelMessage->setPositionY(y);           y -= mpLabelMessage->getContentSize().height;
	mpBoard = cocos2d::DrawNode::create();
	mpBoard->setPositionY(y);                  y -= mBoardSize;
	
	cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(mBoardSize, 0), cocos2d::Point(mBoardSize, -mBoardSize), cocos2d::Point(0, -mBoardSize)};
	mpBoard->drawPolygon(pp, 4, BOARD_COLOR, 0, BOARD_COLOR);

	b0 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Suspend"), CC_CALLBACK_1(View::menuSuspendCallback, this), bs);
	b1 = createButton(cocos2d::Label::createWithBMFont(FONT_MEDIUM, "Abandon"), CC_CALLBACK_1(View::menuAbandonCallback, this), bs);
	b0->setAnchorPoint(cocos2d::Point(0, 0.5f));
	b1->setAnchorPoint(cocos2d::Point(1, 0.5f));
	// bh = set above
	b0->setPosition(marginX, y - bsh2);
	b1->setPosition(mBoardSize - marginX, y - bsh2);

	menu = cocos2d::Menu::create(b0, b1, NULL);
	menu->setPosition(0, 0);

	mpSceneGame->addChild(mpBoard);
	mpSceneGame->addChild(mpLabelMessage);
	mpSceneGame->addChild(menu, Z_TOPMOST);

	mpSceneConnect->setPositionX(0);
	mpSceneLobby->setPositionX(sz.width);
	mpSceneGame->setPositionX(sz.width);
	mpCurrentScene = mpSceneConnect;

}

void View::menuInputUserNameTouchCallback(Ref* pSender)
{
	mpInputUserName->attachWithIME();
}

bool View::onTextFieldInsertText(cocos2d::TextFieldTTF* pSender, const JString& text, size_t nLen)
{
	return false;
}

void View::keyBackClicked(void)
{
	cocos2d::Director::getInstance()->popScene();
}

void View::menuLeaveCallback(Ref* pSender)
{
	if(!mpLbl->leave())
		cocos2d::Director::getInstance()->popScene();
}

const char* View::getInputUserName(void)
{
	return mpInputUserName->getString().c_str();
}

void View::menuConnectCallback(Ref* pSender)
{
	mpLbl->connect(getInputUserName());
}

void View::menuNewGameCallback(Ref* pSender)
{
	mpLbl->createRoom(getInputUserName());
}

void  View::menuRandomGameCallback(Ref* pSender)
{
	mpLbl->joinRandomRoom();
}

void  View::menuRefreshCallback(Ref* pSender)
{
	mpLbl->querySavedRoomList();
}

void  View::menuSuspendCallback(Ref* pSender)
{
	mpLbl->suspendRoom();
}

void  View::menuAbandonCallback(Ref* pSender)
{
	mpLbl->abandonRoom();
}

void View::roomListCallback(const JString& name)
{
	mpLbl->joinRoom(name);
}

void View::savedRoomListCallback(const JString& name)
{
	mpLbl->joinSavedRoom(name);
}

void View::onStateChange(int state, const JString& stateStr, bool inLobby, bool inRoom)
{
	mStateString = stateStr;

	refreshStateInfo(JString());
	if(inLobby)
		switchScene(mpSceneLobby);
	else if(inRoom)
		switchScene(mpSceneGame);
	else
		switchScene(mpSceneConnect);
}
void View::refreshStateInfo(const JString& stateInfo)
{
	mpLabelState->setString((stateInfo?(mStateString+L" "+stateInfo):mStateString).UTF8Representation().cstr());
}

void View::refreshMessage(const JString& msg)
{
	mpLabelMessage->setString(msg.UTF8Representation().cstr());
}

void View::updateRoomList(const JVector<JString>& roomNames)
{
	mpRoomList->removeAllItems();
	for(unsigned int i=0; i<roomNames.getSize(); ++i)
		mpRoomList->addItem(roomNames[i]);
}

void View::updateSavedRoomList(const JVector<JString>& savedRoomNames)
{
	mpSavedRoomList->removeAllItems();
	for(unsigned int i=0; i<savedRoomNames.getSize(); ++i)
		mpSavedRoomList->addItem(savedRoomNames[i]);
}

void View::setupBoard(int tileCols, int tileRows)
{
	this->mTileCols = tileCols;
	this->mTileRows = tileRows;
	EGLOG(DebugLevel::INFO, L"setupBoard", tileCols, tileRows);
	if(mpTiles)
		mpTiles->removeFromParentAndCleanup(true);
	mpTiles = Node::create();
	mpBoard->addChild(mpTiles, Z_TOPMOST);
	
	mTileSize = (mBoardSize - (tileCols * mTileMargin * 2)) / tileCols;
	for(int y = 0; y < tileRows;++y)
		for(int x = 0; x < tileCols;++x)
		{
			Node* tile = Node::create();
			tile->setPosition(mTileMargin + x * (mTileSize + 2 * mTileMargin), - (mTileMargin + ((tileRows - 1 - y) * (mTileSize + 2 * mTileMargin))));
			mpTiles->addChild(tile);
		}
}

void View::updateBoard(unsigned char* tileStates, unsigned char flippedTiles[2])
{
	for(int  i=0; i<mTileCols*mTileRows; ++i)
	{
		Node* tile = (Node*)mpTiles->getChildren().at(i);
		unsigned char tileState = tileStates[i];
		bool flipped = flippedTiles[0] == i || flippedTiles[1] == i;
		if(tile->getChildrenCount() == 0)
		{
			cocos2d::DrawNode* rect = cocos2d::DrawNode::create();
			cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(mTileSize, 0), cocos2d::Point(mTileSize, -mTileSize), cocos2d::Point(0, -mTileSize)};
			rect->drawPolygon(pp, 4, HIDDEN_TILE_COLOR, 0, HIDDEN_TILE_COLOR);
			rect->setAnchorPoint(cocos2d::Point(0, 1));
			Node* icon;
			if(tileState)
			{
				icon = cocos2d::Sprite::create(tileIconFiles[tileState - 1]);
				icon->setScale(mTileSize / icon->getContentSize().width);
				icon->setAnchorPoint(cocos2d::Point(0, 1));
			}
			else
				icon = Node::create(); //dummy

			tile->addChild(rect);
			tile->addChild(icon);
		}
		Node* rect = (Node*)tile->getChildren().at(0);
		Node* icon = (Node*)tile->getChildren().at(1);
		rect->setVisible(tileState != 0 && !flipped);
		icon->setVisible(flipped);
	}
}

int View::sceneOrder(Node* scene)
{
	if(scene == mpSceneConnect) return 1;
	else if(scene == mpSceneLobby) return 2;
	else if(scene == mpSceneGame) return 3;
	else return 0;
}

void View::switchScene(Node* scene)
{
	cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
	float w = sz.width;
	if(mpCurrentScene != scene)
	{
		bool back = false;
		if(mpCurrentScene)
		{
			if(sceneOrder(scene) < sceneOrder(mpCurrentScene))
				back = true;
			if(back)
				mpCurrentScene->runAction(cocos2d::MoveTo::create(0.4f, cocos2d::Point(w, mpCurrentScene->getPositionY())));
			else
				mpCurrentScene->runAction(cocos2d::MoveTo::create(0.4f, cocos2d::Point(-w, mpCurrentScene->getPositionY())));
		}		
		if(back)
			scene->setPositionX(-w);
		else
			scene->setPositionX(w);
		scene->runAction(cocos2d::MoveTo::create(0.4f, cocos2d::Point(0, scene->getPositionY())));

		mpCurrentScene = scene;
	}
}