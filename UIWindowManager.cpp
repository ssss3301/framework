#include "UIWindowManager.h"
#include "Util.h"
#include "UIMask.h"
#include "PrefabsLoader.h"
#include "UIPrefabsReader.h"

#define DefaultWindowLayerZOrder 1000

UIWindowManager* UIWindowManager::_sInstance = nullptr;

UIWindowManager* UIWindowManager::getInstance()
{
	if (_sInstance == nullptr)
		_sInstance = new UIWindowManager();

	return _sInstance;
}

UIWindowManager::UIWindowManager()
:_defaultWindowLayer(nullptr)
, _userWindowLayer(nullptr)
{
	_defaultWindowLayer = cocos2d::Layer::create();
	if (_defaultWindowLayer)
		_defaultWindowLayer->retain();
}

UIWindowManager::~UIWindowManager()
{
	if (_defaultWindowLayer)
		_defaultWindowLayer->release();

	std::map<std::string, UIWindow*>::iterator iter = _allWindow.begin();
	for (; iter != _allWindow.end(); ++iter)
	{
		UIWindow* window = iter->second;
		if (window)
			window->release();
	}

}

UIWindow* UIWindowManager::loadWindow(const std::string& filename)
{
	PrefabsLoader::getInstance()->registReaderObject("UIWindowReader"
		, (cocos2d::ObjectFactory::Instance)UIPrefabsReader<UIWindow>::getInstance);
	UIWindow* pWindow = dynamic_cast<UIWindow*>(PrefabsLoader::getInstance()->createNode(filename));
	return pWindow;
}

void UIWindowManager::loadWindowWithJsonFile(const std::string& jsonFilename)
{
	std::string jsonstr = cocos2d::FileUtils::getInstance()->getStringFromFile(jsonFilename);
	rapidjson::Document jsonDoc;
	jsonDoc.Parse<0>(jsonstr.c_str());

	rapidjson::Value& windowList = jsonDoc["windowList"];
	if (windowList.Capacity() <= 0)
		return;

	PrefabsLoader::getInstance()->registReaderObject("UIWindowReader"
		, (cocos2d::ObjectFactory::Instance)UIPrefabsReader<UIWindow>::getInstance);
	for (int i = 0; i < windowList.Capacity(); ++i)
	{
		rapidjson::Value& windowJson = windowList[i];
		if (!windowJson.HasMember("WindowResource"))
			continue;

		UIWindow* window = (UIWindow*)PrefabsLoader::createNode(windowJson["WindowResource"].GetString());
		if (window == nullptr)
			continue;

		if (windowJson.HasMember("WindowName"))
			window->setName(windowJson["WindowName"].GetString());
		if (windowJson.HasMember("WindowType"))
			window->setWindowType((UIWindowType)windowJson["WindowType"].GetInt());
		if (windowJson.HasMember("AlwaysOnTop"))
			window->setAlwaysOnTop(windowJson["AlwaysOnTop"].GetBool());

		pushWindow(window);
	}
}

void UIWindowManager::showWindow(const std::string& windowName)
{
	cocos2d::Node* windowLayer = getWindowLayer();
	if (windowLayer == nullptr)
	{
		Util::getLogInstance()->Log("not found window layer");
		return;
	}

	UIWindow* window = getWindow(windowName);
	if (window == nullptr)
	{
		Util::getLogInstance()->Log("can not find %s window", windowName.c_str());
		return;
	}

	if (_currentDisplayWindow.size() > 0)
	{
		UIWindow* topWindow = _currentDisplayWindow[_currentDisplayWindow.size() - 1];
		if (topWindow->getWindowType() == UIWindowType_Popup)
		{
			Util::getLogInstance()->Log("already have popup window");
			return;
		}
	}

	UIWindowType windowType = window->getWindowType();
	UIWindowStatus status = window->getWindowStatus();

	if (windowType == UIWindowType_Overlapped || windowType == UIWindowType_Popup)
	{
		if (!isWindowDisplayed(window))
		{
			if (window->isAlwaysOnTop())
			{
				_currentDisplayWindow.push_back(window);
			}
			else
			{
				_currentDisplayWindow.insert(_currentDisplayWindow.begin(), window);
			}
			if (status != UIWindowStatus_Hidden)
				windowLayer->addChild(window);
		}
	}
	else if (windowType == UIWindowType_Standalone)
	{
		hideCurrentDisplayWindow();
		_currentDisplayWindow.push_back(window);
		if (!isWindowDisplayed(window) && status != UIWindowStatus_Hidden)
			windowLayer->addChild(window);
	}
	window->show();
	bubbleWindow(window);

	if (_userWindowLayer == nullptr)
	{
		cocos2d::Node* currentScene = cocos2d::Director::getInstance()->getRunningScene();
		if (currentScene == nullptr)
		{
			Util::getLogInstance()->Log("no scene is running");
			return;
		}

		if (currentScene->getChildByName("_UIWindowLayer") == nullptr)
		{
			currentScene->addChild(_defaultWindowLayer, DefaultWindowLayerZOrder, "_UIWindowLayer");
		}
	}

}

void UIWindowManager::hideWindow(const std::string& windowName)
{
	UIWindow* window = getWindow(windowName);
	if (window == nullptr)
	{
		Util::getLogInstance()->Log("can not find %s window", windowName.c_str());
		return;
	}

	std::vector<UIWindow*>::iterator iter = std::find(_currentDisplayWindow.begin(), _currentDisplayWindow.end()
		, window);
	if (iter != _currentDisplayWindow.end())
	{
		UIWindow* window = *iter;
		if (window)
			window->hide();
		_currentDisplayWindow.erase(iter);
	}
}

void UIWindowManager::pushWindow(UIWindow* window)
{
	if (window == nullptr)
		return;
	std::string windowName = window->getWindowName();
	std::map<std::string, UIWindow*>::const_iterator iter = _allWindow.find(windowName);
	if (iter != _allWindow.end())
	{
		Util::getLogInstance()->Log("%s window is already exist!", windowName.c_str());
		return;
	}

	window->retain();
	_allWindow[windowName] = window;
}

void UIWindowManager::popWindow(UIWindow* window)
{
	if (window == nullptr)
		return;

	//如果在当前显示的列表中则从其中删除
	std::vector<UIWindow*>::iterator currentDisplayIter = std::find(_currentDisplayWindow.begin()
		, _currentDisplayWindow.end(), window);
	if (currentDisplayIter != _currentDisplayWindow.end())
	{
		_currentDisplayWindow.erase(currentDisplayIter);
		cocos2d::Node* windowLayer = getWindowLayer();
		if (windowLayer)
			windowLayer->removeChild(window);
	}

	//从窗口列表中删除
	std::string windowName = window->getWindowName();
	std::map<std::string, UIWindow*>::iterator iter = _allWindow.find(windowName);
	if (iter == _allWindow.end())
	{
		Util::getLogInstance()->Log("%s window not exist!", windowName.c_str());
		return;
	}
	window->release();
	_allWindow.erase(iter);
}

void UIWindowManager::setWindowLayer(cocos2d::Node* windowLayer)
{
	_defaultWindowLayer->removeAllChildren();
	_userWindowLayer->removeAllChildren();
	_currentDisplayWindow.clear();
	_userWindowLayer = windowLayer;
}

cocos2d::Node* UIWindowManager::getWindowLayer()
{
	if (_userWindowLayer == nullptr)
		return _defaultWindowLayer;

	return _userWindowLayer;
}

UIWindow* UIWindowManager::getWindow(const std::string& windowName)
{
	std::map<std::string, UIWindow*>::iterator iter = _allWindow.find(windowName);
	if (iter != _allWindow.end())
		return iter->second;

	return nullptr;
}

void UIWindowManager::bubbleWindow(UIWindow* window)
{
	if (window == nullptr)
		return;

	int startIndex = 0;
	for (int i = 0; i < _currentDisplayWindow.size(); ++i)
	{
		if (_currentDisplayWindow[i] == window)
		{
			startIndex = i;
			break;
		}
	}

	for (int j = startIndex; j < _currentDisplayWindow.size() - 1; ++j)
	{
		UIWindow* nextWindow = _currentDisplayWindow[j + 1];
		if (window->isAlwaysOnTop())
		{
			std::swap(_currentDisplayWindow[j], _currentDisplayWindow[j + 1]);
		}
		else
		{
			if (!nextWindow->isAlwaysOnTop())
				std::swap(_currentDisplayWindow[j], _currentDisplayWindow[j + 1]);
			else
				break;
		}
	}

	for (int k = 0; k < _currentDisplayWindow.size(); ++k)
	{
		UIWindow* window = _currentDisplayWindow[k];
		if (window)
		{
			window->setLocalZOrder(k);
			if (window->getWindowStatus() == UIWindowStatus_Visible && k != _currentDisplayWindow.size() - 1)
				window->frozen();
		}
	}
}

bool UIWindowManager::isWindowDisplayed(UIWindow* window)
{
	std::vector<UIWindow*>::const_iterator findIter = std::find(_currentDisplayWindow.begin()
		, _currentDisplayWindow.end(), window);

	return (findIter != _currentDisplayWindow.end());
}

void UIWindowManager::hideCurrentDisplayWindow()
{
	for (int i = 0; i < _currentDisplayWindow.size(); ++i)
	{
		UIWindow* window = _currentDisplayWindow[i];
		if (window)
		{
			window->hide();
		}
	}

	_currentDisplayWindow.clear();
}
