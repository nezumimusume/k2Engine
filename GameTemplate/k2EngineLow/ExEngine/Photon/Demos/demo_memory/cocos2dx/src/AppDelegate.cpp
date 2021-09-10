#include "cocos2d.h"
#include "AppDelegate.h"
#include "View.h"
#include "ViewConstants.h"
#include "../../src/LoadBalancingListener.h"
#include "../../src/Game.h"
#include "../../src/DemoConstants.h"

#define LOGIC_TICK_INTERVAL 0.01f

class LogicTick : public cocos2d::Node
{
public:
	LogicTick(ExitGames::LoadBalancing::Client* pLbc, LoadBalancingListener* pLbl)
		: mpLbc(pLbc)
		, mpLbl(pLbl)
	{
		schedule(cocos2d::SEL_SCHEDULE(&LogicTick::tick), LOGIC_TICK_INTERVAL);
	}
	void tick(float)
	{
		mpLbc->service();
		mpLbl->service();
	}
private:
	ExitGames::LoadBalancing::Client* mpLbc;
	LoadBalancingListener* mpLbl;
};

AppDelegate::AppDelegate(void)
{
}

AppDelegate::~AppDelegate(void)
{
}

bool AppDelegate::applicationDidFinishLaunching(void)
{
	cocos2d::Director* director = cocos2d::Director::getInstance();
	cocos2d::GLView* glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = cocos2d::GLViewImpl::createWithRect("demo_memory", cocos2d::Rect(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
#else
		glview = cocos2d::GLViewImpl::create("demo_memory");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0f / 60);

	cocos2d::Scene* pRootScene = cocos2d::Scene::create();
	View* pView = View::create();
	pRootScene->addChild(pView);

	LoadBalancingListener* pLbl = new LoadBalancingListener();
	ExitGames::LoadBalancing::Client* pLbc = new ExitGames::LoadBalancing::Client(*pLbl, APP_ID, APP_VERSION);
	pLbc->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	ExitGames::Common::Base::setListener(pLbl);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	pView->init(pLbl);
	pView->setup();
	pLbl->init(pLbc, pView);


	LogicTick* pLogicTick = new LogicTick(pLbc, pLbl);
	pRootScene->addChild(pLogicTick);
	// run
	director->runWithScene(pRootScene);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground(void)
{
	cocos2d::Director::getInstance()->pause();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground(void)
{
	cocos2d::Director::getInstance()->resume();
}