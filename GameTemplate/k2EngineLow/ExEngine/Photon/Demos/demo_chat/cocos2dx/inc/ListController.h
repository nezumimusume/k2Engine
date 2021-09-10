#pragma once

#include "cocos2d.h"
#include "Common-cpp/inc/Common.h"

typedef void (cocos2d::Ref::*ListControllerHandler)(const ExitGames::Common::JString&);

class ListController : public cocos2d::Layer
{
public:
	static ListController* create(float width, float height, cocos2d::Ref* target, ListControllerHandler selector);

	ListController(float width, float height, cocos2d::Ref* target, ListControllerHandler selector);
    ~ListController(void);

	bool addItem(const ExitGames::Common::JString& label);
	bool removeItem(const ExitGames::Common::JString& label);
	void removeAllItems(void);
	void scrollToBottom(void);

    void menuCallback(cocos2d::Ref* pSender);

	//from cocos2d::CCLayer
	virtual bool init(void);
	virtual bool ccTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void ccTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
private:
	void scrollTo(float moveY);
    float beginTouchPosOffset;
    cocos2d::Menu* menu;
	cocos2d::Ref* target;
	ListControllerHandler selector;
	float width;
	float height;
};