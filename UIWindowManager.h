#ifndef __UIWindowManager_H__
#define __UIWindowManager_H__

#include "UIWindow.h"
#include <map>
#include <vector>
#include "cocos2d.h"

class UIWindowManager
{
public:
	static UIWindowManager* getInstance();
	UIWindowManager();
	virtual ~UIWindowManager();

	UIWindow* loadWindow(const std::string& filename);
	void loadWindowWithJson(const std::string& jsonFilename);

	void showWindow(const std::string& windowName);
	void hideWindow(const std::string& windowName);

	void pushWindow(UIWindow* window);
	void popWindow(UIWindow* window);

	void setWindowLayer(cocos2d::Node* windowLayer);
	void hideCurrentDisplayWindow();

private:
	cocos2d::Node* getWindowLayer();
	UIWindow* getWindow(const std::string& windowName);
	void bubbleWindow(UIWindow* window);
	void popCurrentDisplayWindow(UIWindow* window);
	bool isWindowDisplayed(UIWindow* window);

private:
	static UIWindowManager*	_sInstance;
	std::map<std::string, UIWindow*>	_allWindow;
	std::vector<UIWindow*>	_currentDisplayWindow;

	cocos2d::Node*	_defaultWindowLayer;
	cocos2d::Node*	_userWindowLayer;
};

#endif //__UIWindowManager_H__