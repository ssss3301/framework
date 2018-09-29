#include "UIWindow.h"

UIWindow* UIWindow::create()
{
	UIWindow* pWindow = new UIWindow();
	if (pWindow && pWindow->init())
	{
		pWindow->autorelease();
		return pWindow;
	}
	{
		CC_SAFE_DELETE(pWindow);
		pWindow = nullptr;
	}
}

UIWindow* UIWindow::create(UIWindowType type)
{
	UIWindow* pWindow = new UIWindow();
	if (pWindow && pWindow->init(type))
	{
		pWindow->autorelease();
		return pWindow;
	}
	else
	{
		CC_SAFE_DELETE(pWindow);
		pWindow = nullptr;
	}
}

UIWindow* UIWindow::create(UIWindowType type, bool alwaysOnTop)
{
	UIWindow* pWindow = new UIWindow();
	if (pWindow && pWindow->init(type, alwaysOnTop))
	{
		pWindow->autorelease();
		return pWindow;
	}
	else
	{
		CC_SAFE_DELETE(pWindow);
		return nullptr;
	}
}

UIWindow::UIWindow()
: _type(UIWindowType_Overlapped)
, _alwaysOnTop(false)
, _status(UIWindowStatus_Visible)
, _useMask(true)
{

}

UIWindow::UIWindow(UIWindowType type)
: _type(type)
, _alwaysOnTop(type == UIWindowType_Popup)
, _status(UIWindowStatus_Visible)
, _useMask(type != UIWindowType_Overlapped)
{

}

UIWindow::UIWindow(UIWindowType type, bool alwaysOnTop)
: _type(type)
, _alwaysOnTop((type == UIWindowType_Popup) ? true : alwaysOnTop)
, _status(UIWindowStatus_Visible)
, _useMask(type != UIWindowType_Overlapped)
{

}

UIWindow::~UIWindow()
{

}

bool UIWindow::init()
{
	return init(UIWindowType_Overlapped);
}

bool UIWindow::init(UIWindowType type)
{
	return init(type, false);
}

bool UIWindow::init(UIWindowType type, bool alwaysOnTop)
{
	if (!PrefabsBase::init())
		return false;

	_type = type;
	_alwaysOnTop = alwaysOnTop;
}


void UIWindow::setWindowType(UIWindowType type)
{
	_type = type;
}

UIWindowType UIWindow::getWindowType() const
{
	return _type;
}

void UIWindow::show()
{
	_status = UIWindowStatus_Visible;
}

void UIWindow::hide()
{
	_status = UIWindowStatus_Hidden;
}

void UIWindow::frozen()
{
	_status = UIWindowStatus_Freeze;
}

void UIWindow::setAlwaysOnTop(bool onTop)
{
	if (_type == UIWindowType_Popup)
		return;

	_alwaysOnTop = onTop;
}

bool UIWindow::isAlwaysOnTop() const
{
	return _alwaysOnTop;
}

void UIWindow::setWindowName(std::string name)
{
	_name = name;
}

std::string UIWindow::getWindowName() const
{
	return _name;
}

void UIWindow::setWindowStatus(UIWindowStatus status)
{
	_status = status;
}

UIWindowStatus UIWindow::getWindowStatus() const
{
	return _status;
}

bool UIWindow::isUseMask() const
{
	return _useMask;
}
