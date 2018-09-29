#ifndef __UIWindow_H__
#define __UIWindow_H__

#include <string>
#include "cocos2d.h"
#include "PrefabsBase.h"

typedef enum UIWindowType
{
	UIWindowType_Overlapped,
	UIWindowType_Popup,
	UIWindowType_Standalone
};

typedef enum UIWindowStatus
{
	UIWindowStatus_Visible,
	UIWindowStatus_Freeze,
	UIWindowStatus_Hidden
};

class UIWindow : public PrefabsBase
{
public:
	static UIWindow* create();
	static UIWindow* create(UIWindowType type);
	static UIWindow* create(UIWindowType type, bool alwaysOnTop);

	UIWindow();
	UIWindow(UIWindowType type);
	UIWindow(UIWindowType type, bool alwaysOnTop);
	virtual ~UIWindow();

	virtual bool init();
	virtual bool init(UIWindowType type);
	virtual bool init(UIWindowType type, bool alwaysOnTop);

	void setWindowType(UIWindowType type);
	UIWindowType getWindowType() const;

	void setAlwaysOnTop(bool onTop);
	bool isAlwaysOnTop()const;

	void setWindowName(std::string name);
	std::string getWindowName() const;

	void setWindowStatus(UIWindowStatus status);
	UIWindowStatus getWindowStatus() const;

	bool isUseMask() const;

	virtual void show();
	virtual void hide();
	virtual void frozen();

private:
	UIWindowType	_type;
	bool			_alwaysOnTop;
	std::string		_name;
	UIWindowStatus	_status;
	bool			_useMask;
};

#endif //__UIWindow_H__