#include "ListController.h"
#include "ViewConstants.h"

using namespace ExitGames::Common;

// set according to screen size in ListController::init()
float LOG_FONT_SIZE       = 28;
float LOG_LINE_SPACE      = 32;
float LOG_LEFT_MARGIN     = 40;

const char* TOOLTIP_FNT_FILE1 = "arial20.fnt";
const char* TOOLTIP_FNT_FILE2 = "arial28.fnt";

cocos2d::Label* tooltip = NULL;
cocos2d::DrawNode* tooltipBg = NULL;

void clearTooltip(void)
{
	if(tooltip) 
		tooltip->removeFromParentAndCleanup(true);
	if(tooltipBg) 
		tooltipBg->removeFromParentAndCleanup(true);
	tooltip = NULL;
	tooltipBg = NULL;
}

class LogMenuItemLabel : public cocos2d::MenuItemLabel
{
	virtual void selected(void)
	{
		MenuItemLabel::selected();
		
		clearTooltip();
//		CCLayer* layer = CCLayer::create();
	//	getParent()->getParent()->addChild(layer);
		float winWidth = cocos2d::Director::getInstance()->getWinSize().width;
		const float MARGIN = 0.05f;
		float width = winWidth * (1 - 2 * MARGIN);
		tooltip = cocos2d::Label::createWithBMFont(winWidth < 400 ? TOOLTIP_FNT_FILE1 : TOOLTIP_FNT_FILE2, ((cocos2d::Label*)getLabel())->getString());
		tooltip->setLineBreakWithoutSpace(true);
		tooltip->setAnchorPoint(cocos2d::Point(0, 1));
		tooltipBg = cocos2d::DrawNode::create();
		float height = tooltip->getContentSize().height;
		float margin = width / 40;
		cocos2d::Point pp[] = { cocos2d::Point(-margin, margin), cocos2d::Point(width + margin, margin), cocos2d::Point(width + margin, -height - margin), cocos2d::Point(-margin, -height - margin)};
		tooltipBg->drawPolygon(pp, 4, LOG_TOOLTIP_COLOR, 1, LOG_BORDER_COLOR);
		float y = convertToWorldSpace(cocos2d::Point(0,0)).y;
		float y2 = y + height + FONT_SIZE * 2;

		Node* scene = getParent()->getParent()->getParent();
		if(y2 > cocos2d::Director::getInstance()->getWinSize().height - margin)
			y2 = y - FONT_SIZE * 2;
		// attach to scene
 		scene->addChild(tooltipBg);
		tooltipBg->setPosition(winWidth * MARGIN, y2 - scene->getPositionY());
		tooltipBg->addChild(tooltip);
	}

	virtual void unselected(void)
	{
		MenuItemLabel::unselected();
		
		clearTooltip();
	}
};

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
	bg->drawPolygon(pp, 4, LOG_COLOR, 1, LOG_BORDER_COLOR);

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

	float h = cocos2d::Director::getInstance()->getWinSize().height;
	
	LOG_FONT_SIZE       = h * 3.3f / 100;
	LOG_LINE_SPACE      = LOG_FONT_SIZE * 8  /7;
	LOG_LEFT_MARGIN     = h / 20;

	return true;

}

cocos2d::MenuItemLabel* createLogMenuItemLabel(cocos2d::Node* label, const cocos2d::ccMenuCallback& callback)
{
	cocos2d::MenuItemLabel *pRet = new LogMenuItemLabel();
    pRet->initWithLabel(label, callback);
    pRet->autorelease();
    return pRet;
}

bool ListController::addItem(const JString& name)
{
	cocos2d::Label* label = cocos2d::Label::createWithTTF(name.UTF8Representation().cstr(), FONT_NAME, LOG_FONT_SIZE);
	cocos2d::MenuItemLabel* item = createLogMenuItemLabel(label, CC_CALLBACK_1(ListController::menuCallback, this));
    menu->addChild(item);
	item->setPosition(cocos2d::Point(LOG_LEFT_MARGIN, -(float)(menu->getChildrenCount() - 1) * LOG_LINE_SPACE));
	item->setAnchorPoint(cocos2d::Point(0, 1));

	scrollTo(menu->getPositionY());

	return true;
}

bool ListController::removeItem(const ExitGames::Common::JString& name)
{
	int i = 0;
	for(; i<menu->getChildrenCount(); ++i)
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
			item->setPositionY(item->getPositionY() + LOG_LINE_SPACE);
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

void ListController::scrollToBottom(void)
{
	scrollTo((float)menu->getChildrenCount() * LOG_LINE_SPACE);
}

void ListController::menuCallback(Ref* pSender)
{
	cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)pSender;
	cocos2d::Label* l = (cocos2d::Label*)(item->getLabel());
	(target->*selector)(l->getString().c_str());
}

bool ListController::ccTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
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

void ListController::ccTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
	cocos2d::Point touchLocation = touch->getLocation();
    float nextY = touchLocation.y - beginTouchPosOffset;
	scrollTo(nextY);
	beginTouchPosOffset += nextY - menu->getPositionY(); // scroll limits correction
}

void ListController::scrollTo(float nextY)
{
	// leave some space at bottom
	const float height2 = height - LOG_LINE_SPACE/3;
    if(nextY < 0.0f)
    {
        menu->setPositionY(0);
    }
	else 
	{
		float y = menu->getChildrenCount() * LOG_LINE_SPACE - height2;
		if(nextY > y)
			menu->setPositionY(y > 0 ? y : 0);
		else
			menu->setPositionY(nextY);
	}

	// clipping
	const float FADE_START = 0.9f;
	const float FADE_EXPANSION = 10.f;
	const float LABEL_CENTER_FIX = 4.f;
	for(int i=0; i<menu->getChildrenCount(); ++i)
	{
		cocos2d::MenuItemLabel* item = (cocos2d::MenuItemLabel*)(menu->getChildren().at(i));
		float centerOffs = item->getPositionY() + (menu->getPositionY() + height2/2 - LOG_LINE_SPACE/2 + LABEL_CENTER_FIX);
		if(centerOffs < 0) centerOffs = -centerOffs;
		centerOffs = centerOffs / (height2/2 - LOG_LINE_SPACE/2 + FADE_EXPANSION);
		item->setEnabled(centerOffs < 1);
		if(centerOffs < FADE_START)
			item->setOpacity(255);
		else if(centerOffs < 1) 
			item->setOpacity(uint8_t(255 * (1 - centerOffs) / (1 - FADE_START)));
		else
			item->setOpacity(0);
	}
}