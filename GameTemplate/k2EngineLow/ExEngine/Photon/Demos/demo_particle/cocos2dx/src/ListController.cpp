#include "ListController.h"
#include "ViewConstants.h"

#define FONT "arial28.fnt"
#define LINE_SPACE          32
#define LEFT_MARGIN         40


ListController* ListController::create(float width, float height, Ref* target, ListControllerHandler selector)
{
	ListController* pRet = new ListController(width, height, target, selector);
	pRet->init();
	pRet->autorelease();

	return pRet;
}

ListController::ListController(float width, float height, Ref* target, ListControllerHandler selector)
: beginTouchPosOffset(0)
, width(width)
, height(height)
, target(target)
, selector(selector)
{
	cocos2d::DrawNode* bg = cocos2d::DrawNode::create();
	bg->setPosition(0, 0);	
	cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(width, 0), cocos2d::Point(width, -height), cocos2d::Point(0, -height)};
	bg->drawPolygon(pp, 4, ROOM_LIST_BG_COLOR, 0, GRID_COLOR);

	addChild(bg);
	menu = cocos2d::Menu::create();
	menu->setPosition(0, 0);	
	addChild(menu);
	setContentSize(cocos2d::Size(width, height));
}

ListController::~ListController(void)
{
}

bool ListController::init(void)
{
	if (!Layer::init())
		return false;

	auto listener = cocos2d::EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(ListController::onTouchesBegan, this);

	auto dispatcher = this->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	scrollTo(0);
	return true;

}

bool ListController::addItem(const ExitGames::Common::JString& name)
{
	for(int i=0; i<menu->getChildrenCount(); ++i)
	{
		cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)(menu->getChildren().at(i));
		cocos2d::Label* l = (cocos2d::Label*)(item->getLabel());
		if(l->getString().c_str() == name)
			return false;
	}
	cocos2d::Label* label = cocos2d::Label::createWithBMFont(FONT, name.UTF8Representation().cstr());
	label->setAlignment(cocos2d::TextHAlignment::LEFT);
	cocos2d::MenuItemLabel* item = cocos2d::MenuItemLabel::create(label, CC_CALLBACK_1(ListController::menuCallback, this));

	menu->addChild(item);
	item->setPosition(cocos2d::Point(LEFT_MARGIN, -(float)(menu->getChildrenCount() - 1) * LINE_SPACE - LINE_SPACE/2));
	item->setAnchorPoint(cocos2d::Point(0, 0.5));

	cocos2d::DrawNode* border = cocos2d::DrawNode::create();
	border->setTag(1);
	border->setPositionX(-LEFT_MARGIN);
	item->addChild(border);

	scrollTo(menu->getPositionY());

	return true;
}

bool ListController::removeItem(const ExitGames::Common::JString& name)
{
	int i = 0;
	for(; i < menu->getChildrenCount(); ++i)
	{
		cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)(menu->getChildren().at(i));
		cocos2d::Label* l = (cocos2d::Label*)(item->getLabel());
		if(l->getString().c_str() == name)
		{
			item->removeFromParentAndCleanup(true);
			break;
		}
	}
	bool ret = false;
	if(i < menu->getChildrenCount())
	{
		for(; i<menu->getChildrenCount(); ++i)
		{
			cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)(menu->getChildren().at(i));
			item->setPositionY(item->getPositionY() + LINE_SPACE);
		}
		ret = true;
	}

	scrollTo(menu->getPositionY());

	return ret;
}

void ListController::removeAllItems(void)
{
	menu->removeAllChildrenWithCleanup(true);
	scrollTo(menu->getPositionY());
}

void ListController::menuCallback(Ref* pSender)
{
	cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)pSender;
	cocos2d::Label* l = (cocos2d::Label*)(item->getLabel());
	(target->*selector)(l->getString().c_str());
}

bool ListController::TouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	cocos2d::Point touchLocation = touch->getLocation();
	cocos2d::Point pos = convertToNodeSpace(touchLocation);
	if(pos.x >= 0 && pos.x < width && pos.y <= 0 && pos.y > -height)
	{
		beginTouchPosOffset = touch->getLocation().y - menu->getPositionY();
		return true;
	}
	else
		return false;
}

void ListController::TouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
	cocos2d::Point touchLocation = touch->getLocation();
	float nextY = touchLocation.y - beginTouchPosOffset;
	scrollTo(nextY);
	beginTouchPosOffset += nextY - menu->getPositionY(); // scroll limits correction
}

void ListController::scrollTo(float nextY)
{
	// leave some space at bottom
	const float height2 = height - LINE_SPACE/3;
	if(nextY < 0.0f)
		menu->setPositionY(0);
	else 
	{
		float y = menu->getChildrenCount() * LINE_SPACE - height2;
		if(nextY > y)
			menu->setPositionY(y > 0 ? y : 0);
		else
			menu->setPositionY(nextY);
	}

	// clipping
	const float FADE_START = 0.9f;
	const float FADE_EXPANSION = 10;
	const float LABEL_CENTER_FIX = 4;
	for(int i=0; i<menu->getChildrenCount(); ++i)
	{
		cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)(menu->getChildren().at(i));
		float centerOffs = item->getPositionY() + (menu->getPositionY() + height2/2 + LABEL_CENTER_FIX);
		if(centerOffs < 0) centerOffs = -centerOffs;
		centerOffs = centerOffs / (height2/2 - LINE_SPACE/2 + FADE_EXPANSION);
		item->setEnabled(centerOffs < 1);
		unsigned char op = 0;
		if(centerOffs < FADE_START)
			op = 255;
		else if(centerOffs < 1) 
			op = static_cast<unsigned char>(255 * (1 - centerOffs) / (1 - FADE_START));
		else
			op = 0;
		item->setOpacity(op);

		cocos2d::DrawNode* border = (cocos2d::DrawNode*)item->getChildByTag(1);
		border->clear();
		float w = getContentSize().width;
		cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point(w, 0), cocos2d::Point(w, 0), cocos2d::Point(0, 0)};
		cocos2d::Color4F color = {1, 1, 1, op/512.0f};
		border->drawPolygon(pp, 4, TRANSPARENT_COLOR, 1, color);
	}
}