#ifndef __ILogCocos2dx_H__
#define __ILogCocos2dx_H__

#include "ILog.h"

class ILogCocos2dx : public ILog
{
public:
	ILogCocos2dx();
	virtual ~ILogCocos2dx();
	virtual void Log(char* fmt, ...);
};

#endif //__ILogCocos2dx_H__