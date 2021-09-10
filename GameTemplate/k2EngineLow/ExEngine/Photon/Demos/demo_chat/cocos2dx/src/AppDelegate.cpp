#include "cocos2d.h"

#include "Chat-cpp/inc/Listener.h"
#include "../../src/DemoConstants.h"
#include "../inc/AppDelegate.h"
#include "../inc/DemoScene.h"
#include "../inc/ViewConstants.h"

bool useTcp = false;

using namespace ExitGames::Chat;
using namespace ExitGames::Common;
using namespace ExitGames::Photon;

#define SERVICE_TICK_INTERVAL 0.01f

class ServiceTick : public cocos2d::Node
{
public:
	ServiceTick(Client* pClient)
		: pClient(pClient)
	{
		schedule(cocos2d::SEL_SCHEDULE(&ServiceTick::tick), SERVICE_TICK_INTERVAL);
	}
	void tick(float)
	{
		pClient->service();
	}
private:
	Client* pClient;
};

AppDelegate::AppDelegate(void)
{
}

AppDelegate::~AppDelegate(void)
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

	cocos2d::GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching(void)
{
	Client* pClient = NULL;
	cocos2d::Director* director = cocos2d::Director::getInstance();
	cocos2d::GLView* glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = cocos2d::GLViewImpl::createWithRect("demo_chat", cocos2d::Rect(0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT));
#else
		glview = cocos2d::GLViewImpl::create("demo_chat");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0f / 60);

	// create a pScene. it's an autorelease object
	cocos2d::Scene* pRootScene = cocos2d::Scene::create();
	DemoScene* pScene = DemoScene::create();
	pRootScene->addChild(pScene);

	pClient = new Client(*pScene, APP_ID, APP_VERSION, ConnectionProtocol::DEFAULT);
	pClient->setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	ExitGames::Common::Base::setListener(pScene);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	pScene->setClient(pClient);
	pScene->setupUI();

	ServiceTick* serviceTick = new ServiceTick(pClient);
	pRootScene->addChild(serviceTick);
	// run
	director->runWithScene(pRootScene);

	// trigger start hint
	pScene->onStateChange(pClient->getState());

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