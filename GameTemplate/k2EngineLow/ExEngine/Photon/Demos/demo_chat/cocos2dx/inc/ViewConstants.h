#pragma once

#include "cocos2d.h"

static const int SCREEN_WIDTH = 512;
static const int SCREEN_HEIGHT = 800;

static const cocos2d::Color3B INPUT_COLOR = {0, 0, 255};
static const cocos2d::Color4F LOG_COLOR = {0.2f, 0.2f, 0.2f, 1};
static const cocos2d::Color4F LOG_TOOLTIP_COLOR = {0.1f, 0.1f, 0.1f, 1};
static const cocos2d::Color4F LOG_BORDER_COLOR = {0.7f, 0.7f, 0.7f, 1};
static const cocos2d::Color4F TRANSPARENT_COLOR = {0, 0, 0, 0};
static const cocos2d::Color4F BUTTON_COLOR = {1, 1, 1, 0.15f};
static const cocos2d::Color4F BUTTON_BORDER_COLOR = {0.7f, 0.7f, 0.7f, 0.7f};

static const char* FONT_NAME    = "arial.ttf";
// set according to screen size in DemoScene::init()
extern float FONT_SIZE;