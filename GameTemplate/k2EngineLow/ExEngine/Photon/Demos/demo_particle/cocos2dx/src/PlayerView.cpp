#include "PlayerView.h"
#include "ViewConstants.h"

using namespace ExitGames::Common;

PlayerView::PlayerView(int number, const JString& name, int cellSize, float borderWidth)
	: number(number)
	, name(name)
	, cellSize(cellSize)
	, borderWidth(borderWidth)
	, x(0)
	, y(0)
	, label(NULL)
	, opacity(255)
{
	updateView(cellSize);
	setPos(0, 0);
	setColor(cocos2d::Color3B::BLUE);
}

const JString& PlayerView::getName(void)
{
	return name;
}

void PlayerView::setPos(int x, int y)
{
	this->x = x;
	this->y = y;
	setPosition(cocos2d::Point((float)x*cellSize, -(float)y*cellSize));
}

void PlayerView::setColor(int color)
{
	setColor(cocos2d::Color3B(color >> 0x10, color >> 8, color));
}

void PlayerView::setColor(const cocos2d::Color3B& color)
{
	this->color = color;
	sprite->setColor(color);
}

int PlayerView::getColor(void)
{
	return (color.r<<16) | (color.g<<8) | color.b;
}

void PlayerView::setNumber(int number)
{
	this->number = number;
}

void PlayerView::fadeOpacity(float v)
{
	opacity -= v * 256;
	if(opacity < 0) opacity = 0;
	sprite->setOpacity((uint8_t)opacity);
	label->setOpacity((uint8_t)opacity);
}

void PlayerView::resetOpacity(void)
{
	opacity = 255;
	sprite->setOpacity((uint8_t)opacity);
	label->setOpacity((uint8_t)opacity);
}

int PlayerView::getX(void)
{
	return x;
}

int PlayerView::getY(void)
{
	return y;
}

int PlayerView::getNumber(void)
 {
	 return number;
}

void PlayerView::updateView(int size)
{
	removeAllChildrenWithCleanup(true);
	cellSize = size;
	sprite = cocos2d::Sprite::create();
	sprite->setTextureRect(cocos2d::Rect(0, 0, (float)cellSize, (float)cellSize));
	sprite->setAnchorPoint(cocos2d::Point(0, 1));
	addChild(sprite);

	if(borderWidth)
	{
		cocos2d::DrawNode* b = cocos2d::DrawNode::create();
		cocos2d::Point pp[] = { cocos2d::Point(0, 0), cocos2d::Point((float)cellSize, 0), cocos2d::Point((float)cellSize, -(float)cellSize), cocos2d::Point(0, -(float)cellSize)};
		b->drawPolygon(pp, 4, TRANSPARENT_COLOR, (float)borderWidth, BLACK_COLOR);
		addChild(b);
	}

	label = cocos2d::Label::createWithBMFont("arial20.fnt", (name + "/" + number).UTF8Representation().cstr());
	label->setAnchorPoint(cocos2d::Point(0, 1));
	label->setColor(cocos2d::Color3B::BLACK);
	addChild(label);
	setColor(color);
}