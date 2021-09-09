#include "cocos2d.h"
#include "AppDelegate.h"
#include "Console.h"
#include "DemoScene.h"
#include "ViewConstants.h"
#include "../../src/DemoConstants.h"
#include "../../src/LoadBalancingListener.h"

bool useTcp = false;

using namespace ExitGames::LoadBalancing;
using namespace ExitGames::Common;
using namespace ExitGames::Photon;

#define LOGIC_TICK_INTERVAL 0.01f

class LogicTick : public cocos2d::Node
{
public:
	LogicTick(Client* pLbc, LoadBalancingListener* pLbl, DemoScene* pScene)
		: mpLbc(pLbc)
		, mpLbl(pLbl)
		, mpScene(pScene)
	{
		schedule(cocos2d::SEL_SCHEDULE(&LogicTick::tick), LOGIC_TICK_INTERVAL);
	}
	void tick(float)
	{
		mpLbc->service();
		mpLbl->service();
		mpScene->tick(LOGIC_TICK_INTERVAL);
	}
private:
	Client* mpLbc;
	LoadBalancingListener* mpLbl;
	DemoScene* mpScene;
};

AppDelegate::AppDelegate(void)
{
}

AppDelegate::~AppDelegate(void)
{
}

bool AppDelegate::applicationDidFinishLaunching(void)
{
	LoadBalancingListener* pLbl = NULL;
	Client* pLbc = NULL;

	cocos2d::Director* director = cocos2d::Director::getInstance();
	cocos2d::GLView* glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = cocos2d::GLViewImpl::createWithRect("demo_particle", cocos2d::Rect(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
#else
		glview = cocos2d::GLViewImpl::create("demo_particle");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
//	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0f / 60);

	// create a pScene. it's an autorelease object
	cocos2d::Scene* pRootScene = cocos2d::Scene::create();
	DemoScene* pScene = DemoScene::create();
	pRootScene->addChild(pScene);

	pLbl = new LoadBalancingListener(pScene);
	pLbc = new Client(*pLbl, appID, appVersion, useTcp);
	pLbc->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	ExitGames::Common::Base::setListener(pLbl);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
//	pLbc->setCRCEnabled(true);
	pLbl->setLBC(pLbc);
	pLbc->setTrafficStatsEnabled(true);
//	pLbc->setLimitOfUnreliableCommands(15);
	pScene->setLBC(pLbc, pLbl);
	pScene->setupUI();

	Console::get().writeLine(L"LBC: connecting...");
	pLbl->connect(JString(L"NV")+GETTIMEMS());
	LogicTick* logicTick = new LogicTick(pLbc, pLbl, pScene);
	pRootScene->addChild(logicTick);
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