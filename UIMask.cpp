#include "UIMask.h"

UIMask* UIMask::create()
{
	UIMask* mask = new UIMask();
	if (mask && mask->init())
	{
		mask->autorelease();
		return mask;
	}
	else
	{
		CC_SAFE_DELETE(mask);
		return nullptr;
	}
}

UIMask::UIMask()
: _touchListener(nullptr)
, _owner(nullptr)
{

}

UIMask::~UIMask()
{

}

bool UIMask::init()
{
	cocos2d::Color4B black(0, 0, 0, 128);
	cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();

	if (!cocos2d::LayerColor::initWithColor(black, winSize.width, winSize.height))
		return false;

	_touchListener = cocos2d::EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = CC_CALLBACK_2(UIMask::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(UIMask::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(UIMask::onTouchEnded, this);
	_touchListener->onTouchCancelled = CC_CALLBACK_2(UIMask::onTouchCancelled, this);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);

	return true;
}

bool UIMask::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (event)
		event->stopPropagation();
	return true;
}

void UIMask::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
	
}

void UIMask::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void UIMask::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void UIMask::setMaskColor(const cocos2d::Color3B& color)
{
	cocos2d::LayerColor::setColor(color);
}

void UIMask::setOwner(cocos2d::Node* owner)
{
	_owner = owner;
}
