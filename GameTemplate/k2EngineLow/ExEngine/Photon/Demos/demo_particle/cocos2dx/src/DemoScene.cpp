#include "Console.h"
#include "DemoScene.h"
#include "ViewConstants.h"
#include "../../src/DemoConstants.h"

using namespace ExitGames::Common;

#define Z_TOPMOST   100

#define LABEL_HEIGHT (2 * 15)

cocos2d::Label* gpLabelLog = NULL;

class ConsoleImplementation
{
public:
	void write(const ExitGames::Common::JString& str)
	{
		gpLabelLog->setString(str.UTF8Representation().cstr());
	}

	void writeLine(const ExitGames::Common::JString& str)
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

void Console::write(const ExitGames::Common::JString& str)
{
	mpImp->write(str);
}

void Console::writeLine(const ExitGames::Common::JString& str)
{
	mpImp->writeLine(str);
}

Console& Console::get(void)
{
	static Console console;
	return console;
}

typedef void (cocos2d::Ref::*CheckBoxHandler)(bool);

// setContentSize does not work as expected, returning size by parameter
static cocos2d::MenuItem* decorateButton(cocos2d::MenuItemLabel* item, cocos2d::Size& size)
{
	const cocos2d::Color4F BORDER_COLOR = {1, 1, 1, 1};
	const cocos2d::Color4F NORMAL_COLOR = {1, 1, 1, 0.3f};

	cocos2d::DrawNode* bg = cocos2d::DrawNode::create();
	bg->setPosition(0, 0);
	float lw = item->getLabel()->getContentSize().width;
	float lh = item->getLabel()->getContentSize().height;
	float marginX = lh*0.4f;
	float marginY = lh*0.2f;
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

class CheckBox : public cocos2d::MenuItemLabel
{
public:
	static cocos2d::MenuItemLabel* create(cocos2d::Label* label, Ref* target, CheckBoxHandler selector, bool checked, cocos2d::Size& size)
	{
		CheckBox* pRet = new CheckBox();
		pRet->initCheckbox(label, target, selector);
		pRet->setChecked(checked);
		// init after label size updated
		pRet->initWithLabel(label, NULL);
		decorateButton(pRet, size);
		pRet->autorelease();

		return pRet;
	}
	virtual void activate(void)
	{	
		setChecked(!checked);
		(target->*selector)(checked);
	}
	void setChecked(bool v)
	{
		checked = v;
		label->setString((checked ? uncheckedString + " X" : uncheckedString + "  ").c_str());
	}
	void initCheckbox(cocos2d::Label* label, Ref* target, CheckBoxHandler selector)
	{
		this->label = label;
		this->target = target;
		this->selector = selector;
		uncheckedString = label->getString();
		uncheckedString += ":";
	}
private:
	CheckBoxHandler selector;
	Ref* target;
	cocos2d::Label* label;
	bool checked;
	std::string uncheckedString;
};

DemoScene::DemoScene(void)
	: mpCanvas(NULL)
	, mpGrid(NULL) 
	, mpChecker(NULL)
	, mpPlayersNode(NULL)
	, mpLabelState(NULL)
	, mCanvasSize(1)
	, mCellSize(1)
{
}

DemoScene::~DemoScene(void)
{
}

DemoScene* DemoScene::create(void)
{
	DemoScene* pRet = new DemoScene();
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
	if (!Layer::init())
	{
		return false;
	}

	auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(DemoScene::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(DemoScene::onTouchesMoved, this);

	auto dispatcher = this->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}


void DemoScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	onTouch(touches[0], event);
}

void DemoScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	onTouch(touches[0], event);
}

void DemoScene::onTouch(cocos2d::Touch* touch, cocos2d::Event* event)
{
	cocos2d::Point touchLocation = touch->getLocation();
	cocos2d::Point pos = mpCanvas->convertToNodeSpace(touchLocation);
	mpLbl->setLocalPlayerPos((int)pos.x/mCellSize, -(int)pos.y/mCellSize);
}

class MoveDesktopButton : public cocos2d::Layer
{
public:
	static MoveDesktopButton* create(float y, float height)
	{
		MoveDesktopButton* pRet = new MoveDesktopButton(y, height);
		pRet->init();
		pRet->autorelease();

		return pRet;
	}
	MoveDesktopButton(float y, float height) : height(height)
	{
		setPositionY(y);

		float width = cocos2d::Director::getInstance()->getWinSize().width;
		cocos2d::DrawNode* bg = cocos2d::DrawNode::create();
		bg->setPosition(0, 0);	
		cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(width, 0), cocos2d::Point(width, -height), cocos2d::Point(0, -height)};
		bg->drawPolygon(pp, 4, MOVE_DESKTOP_BUTTON_COLOR, 2, BLACK_COLOR);
		addChild(bg);

		cocos2d::Label* l = cocos2d::Label::createWithBMFont("arial28.fnt", ": : : : : :", cocos2d::TextHAlignment::CENTER, (int)width);
		l->setColor(cocos2d::Color3B(0, 0, 0));
		l->setPosition(width / 2, -height / 2);
		addChild(l);
	}
	//from cocos2d::Layer
	virtual bool init(void)
	{
		if (!Layer::init())
		{
			return false;
		}

		auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
		listener->onTouchesBegan = CC_CALLBACK_2(MoveDesktopButton::onTouchesBegan, this);
		listener->onTouchesBegan = CC_CALLBACK_2(MoveDesktopButton::onTouchesMoved, this);

		auto dispatcher = this->getEventDispatcher();
		dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		return true;

	}
	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
	{
		cocos2d::Point touchLocation = touches[0]->getLocation();    
		cocos2d::Point pos = convertToNodeSpace(touchLocation);
		//return pos.y <= 0 && pos.y > -height;
	}
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
	{
		float y = getParent()->getPositionY() + touches[0]->getLocation().y - touches[0]->getPreviousLocation().y;
		cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
		if(y > 0 && y < getParent()->getContentSize().height - sz.height)
			getParent()->setPositionY(getParent()->getPositionY() + touches[0]->getLocation().y - touches[0]->getPreviousLocation().y);
	}
private:
	float height;
};

float DemoScene::setupDesktop(bool movable)
{
	cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
	mCanvasSize = (int)(sz.width < sz.height ? sz.width : sz.height);

	float y = sz.height;

	mpLabelState = cocos2d::Label::createWithBMFont("arial28.fnt", "Undef", cocos2d::TextHAlignment::LEFT, mCanvasSize);
	mpLabelState->setAnchorPoint(cocos2d::Point(0, 1));
	mpLabelState->setPosition(0, sz.height);
	mpLabelState->setLineBreakWithoutSpace(true);
	addChild(mpLabelState);
		
	y -= mpLabelState->getContentSize().height;
	mpCanvas = cocos2d::DrawNode::create();
	mpCanvas->setPosition(0, y);

	cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point((float)mCanvasSize, 0), cocos2d::Point((float)mCanvasSize, -(float)mCanvasSize), cocos2d::Point(0, -(float)mCanvasSize)};
	mpCanvas->drawPolygon(pp, 4, CANVAS_COLOR, 0, CANVAS_COLOR);

	addChild(mpCanvas);
	mpPlayersNode = Node::create();
	mpCanvas->addChild(mpPlayersNode, Z_TOPMOST);

	y -= mCanvasSize;

	if(movable)
	{
		const float height = 60;
		addChild(MoveDesktopButton::create(y, height));
		y -= height;
	}
	cocos2d::Size bs;
	cocos2d::MenuItem* b0 = createButton(cocos2d::Label::createWithBMFont("arial28.fnt", "New Game"), CC_CALLBACK_1(DemoScene::menuNewGameCallback, this), bs);
	cocos2d::MenuItem* b1 = createButton(cocos2d::Label::createWithBMFont("arial28.fnt", "Leave"), CC_CALLBACK_1(DemoScene::menuLeaveCallback, this), bs);
	cocos2d::MenuItem* b2 = createButton(cocos2d::Label::createWithBMFont("arial28.fnt", "Grid Size"), CC_CALLBACK_1(DemoScene::menuGridSizeCallback, this), bs);
	cocos2d::MenuItem* b3 = createButton(cocos2d::Label::createWithBMFont("arial28.fnt", "Random Color"), CC_CALLBACK_1(DemoScene::menuRandomColorCallback, this), bs);
	cocos2d::MenuItem* b4 = CheckBox::create(cocos2d::Label::createWithBMFont("arial28.fnt", "Automove"), this, (CheckBoxHandler)&DemoScene::menuAutomoveCallback, mpLbl->getAutomove(), bs);
	cocos2d::MenuItem* b5 = CheckBox::create(cocos2d::Label::createWithBMFont("arial28.fnt", "Groups"), this, (CheckBoxHandler)&DemoScene::menuUseGroupsCallback, mpLbl->getUseGroups(), bs);
	float bh = bs.height;

	b0->setAnchorPoint(cocos2d::Point(0, 0.5));
	b1->setAnchorPoint(cocos2d::Point(1, 0.5));
	b2->setAnchorPoint(cocos2d::Point(0, 0.5));
	b3->setAnchorPoint(cocos2d::Point(1, 0.5));
	b4->setAnchorPoint(cocos2d::Point(0, 0.5));
	b5->setAnchorPoint(cocos2d::Point(1, 0.5));
	float marginX = bh*0.3f;
	b0->setPosition(cocos2d::Point(marginX, -bh/2));
	b1->setPosition(cocos2d::Point(mCanvasSize - marginX, -bh/2));
	b2->setPosition(cocos2d::Point(marginX, -bh - bh/2));
	b3->setPosition(cocos2d::Point(mCanvasSize - marginX, -bh - bh/2));
	b4->setPosition(cocos2d::Point(marginX, -bh*2 - bh/2));
	b5->setPosition(cocos2d::Point(mCanvasSize - marginX, -bh*2 - bh/2));

	cocos2d::Menu* pMenu = cocos2d::Menu::create(b0, b1, b2, b3, b4, b5, NULL);
	pMenu->setPosition(0, y);
	addChild(pMenu, Z_TOPMOST);

	y -= bh*3; // 3 button rows
	y -= bh*0.1f; // and some space
	
	float roomListHeight = (float)bh*4;
	mpRoomList = ListController::create(sz.width, roomListHeight, this, (ListControllerHandler)&DemoScene::roomListCallback);
	addChild(mpRoomList);
	mpRoomList->setPosition(0, y);
	mpRoomList->addItem("item-1");
	mpRoomList->addItem("item-2");
	mpRoomList->addItem("item-3");
	mpRoomList->addItem("item-4");
	mpRoomList->addItem("item-5");
	mpRoomList->addItem("item-6");
	mpRoomList->addItem("item-7");
	mpRoomList->addItem("item-8");

	y -= roomListHeight;

	gpLabelLog = cocos2d::Label::createWithBMFont("arial20.fnt", "Undef", cocos2d::TextHAlignment::LEFT, mCanvasSize);
	gpLabelLog->setAnchorPoint(cocos2d::Point(0, 1));
	gpLabelLog->setPosition(0, y);
	addChild(gpLabelLog);

	y -= gpLabelLog->getContentSize().height * 5; //extra lines for log

	return y;
}

void DemoScene::setupUI(void)
{
	if(setupDesktop(false) < 0)
	{
		removeAllChildrenWithCleanup(true);
		float extra = setupDesktop(true);
		cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
		setContentSize(cocos2d::Size(sz.width, sz.height - extra));
	}
	setupScene(DEFAULT_GRID_SIZE);
}

void DemoScene::setLBC(ExitGames::LoadBalancing::Client* lbc, LoadBalancingListener* lbl) 
{
	this->mpLbc = lbc;
	this->mpLbl = lbl;
}

void DemoScene::setupScene(int gridSize)
{
	mCellSize = mCanvasSize / gridSize;
	drawChecker(gridSize);
	drawGrid(gridSize);
	updatePlayerCellSizes(mCellSize);
}

void DemoScene::drawChecker(int gridSize)
{
	if(mpChecker)
		mpChecker->removeFromParentAndCleanup(true);
	mpChecker = cocos2d::DrawNode::create();
	mpChecker->setVisible(mpLbl->getUseGroups());
	mpCanvas->addChild(mpChecker);

	for(int x=0; x<GROUPS_PER_AXIS; ++x)
	{
		for(int y=0; y<GROUPS_PER_AXIS; ++y)
		{
			if((x%2) != (y%2)) 
			{
				float x0 = (float)mCellSize * (x *  gridSize / GROUPS_PER_AXIS);
				float y0 = -(float)mCellSize * (y * gridSize / GROUPS_PER_AXIS);
				float x1 = (float)mCellSize * ((x + 1) * gridSize / GROUPS_PER_AXIS);
				float y1 = -(float)mCellSize * ((y + 1) * gridSize / GROUPS_PER_AXIS);
				cocos2d::Point pp[] = { cocos2d::Point(x0, y0), cocos2d::Point(x1, y0), cocos2d::Point(x1, y1), cocos2d::Point(x0, y1)};
				mpChecker->drawPolygon(pp, 4, CHECKER_COLOR, 0, CHECKER_COLOR);
			}
		}
	}
}

void DemoScene::drawGrid(int gridSize)
{
	if(mpGrid)
		mpGrid->removeFromParentAndCleanup(true);
	mpGrid = cocos2d::DrawNode::create();
	mpCanvas->addChild(mpGrid);
	const float r = LINE_WIDTH/2.0f;
	for(int x=0; x<gridSize+1; ++x)
		mpGrid->drawSegment(cocos2d::Point((float)x*mCellSize, -(float)gridSize*mCellSize), cocos2d::Point((float)x*mCellSize, 0), r, GRID_COLOR);

	for(int y=0; y<gridSize+1; ++y)
		mpGrid->drawSegment(cocos2d::Point(0, - (float)y*mCellSize), cocos2d::Point((float)gridSize*mCellSize, -(float)y*mCellSize), r, GRID_COLOR);
}

void DemoScene::updatePlayerCellSizes(int gridSize)
{
	for(int i=0; i<mpPlayersNode->getChildrenCount(); ++i)
	{
		PlayerView* p = (PlayerView*)mpPlayersNode->getChildren().at(i);
		p->updateView(gridSize);
	}
}

void DemoScene::menuNewGameCallback(Ref* pSender)
{
	mpLbl->createRoom();
}

void DemoScene::menuLeaveCallback(Ref* pSender)
{
	if(mpLbc->getIsInGameRoom())
		mpLbc->opLeaveRoom();
	else
		cocos2d::Director::getInstance()->popScene();
}

void DemoScene::menuGridSizeCallback(Ref* pSender)
{
	mpLbl->nextGridSize();
}

void DemoScene::menuRandomColorCallback(Ref* pSender)
{
	mpLbl->changeRandomColor();
}

void DemoScene::menuAutomoveCallback(bool automove)
{
	mpLbl->setAutomove(automove);
}

void DemoScene::menuUseGroupsCallback(bool useGroups)
{
	mpLbl->setUseGroups(useGroups);
	mpChecker->setVisible(useGroups);
	mpLbl->updateGroups();
}

void DemoScene::roomListCallback(const JString& name)
{
	mpLbc->opJoinRoom(name);
}

void DemoScene::updateState(int state, const JString& stateStr, const JString& joinedRoomName)
{
	mpLabelState->setString((JString() + state + L"/" + stateStr + joinedRoomName?L" "+joinedRoomName:L"").UTF8Representation().cstr());
}

void DemoScene::initPlayers(void) 
{
	mpPlayersNode->removeAllChildrenWithCleanup(true);
}

void DemoScene::addPlayer(int playerNr, const JString& playerName, bool local)
{
	removePlayer(playerNr);
	PlayerView* p = new PlayerView(playerNr, playerName, mCellSize, local ? 1.0f : 0);
	mpPlayersNode->addChild(p, 0, playerNr);
	p->setNumber(playerNr);
}

void DemoScene::removePlayer(int playerNr)
{
	mpPlayersNode->removeChildByTag(playerNr, true);
}

void DemoScene::changePlayerColor(int playerNr, int color)
{
	PlayerView* p = (PlayerView*)mpPlayersNode->getChildByTag(playerNr);
	if(p)
		p->setColor(color);
}

void DemoScene::changePlayerPos(int playerNr, int x, int y)
{
	PlayerView* p = static_cast<PlayerView*>(mpPlayersNode->getChildByTag(playerNr));
	if(p)
	{
		p->setPos(x, y);
		p->resetOpacity();
	}
}

void DemoScene::updateRoomList(const JVector<JString>& roomNames)
{
	mpRoomList->removeAllItems();
	for(unsigned int i=0; i<roomNames.getSize(); ++i)
		mpRoomList->addItem(roomNames[i]);
}

void DemoScene::tick(float interval)
{
	for(int i=0; i<mpPlayersNode->getChildrenCount(); ++i)
	{
		PlayerView* p = (PlayerView*)mpPlayersNode->getChildren().at(i);
		p->fadeOpacity(interval / PLAYER_FADE_TIME);
	}
}