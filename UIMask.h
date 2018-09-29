#ifndef __TouchMask_H__
#define __TouchMask_H__

#include "cocos2d.h"

class UIMask : public cocos2d::LayerColor
{
public:
	static UIMask* create();
	UIMask();
	virtual ~UIMask();

	virtual bool init();
	void setMaskColor(const cocos2d::Color3B& color);
	void setOwner(cocos2d::Node* owner);

protected:
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

private:
	cocos2d::EventListenerTouchOneByOne*	_touchListener;
	cocos2d::Node*	_owner;
};

#endif //__TouchMask_H__