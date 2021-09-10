#pragma once

#include "CCApplication.h"
#include "Common-cpp/inc/Common.h"

/**
@brief	The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate(void);
    virtual ~AppDelegate(void);
    
    /**
    @brief	Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching(void);

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground(void);

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
	virtual void applicationWillEnterForeground(void);
private:
	ExitGames::Common::Logger mLogger;
};