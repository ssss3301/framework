#ifndef __Util_H__
#define __Util_H__

#include "ILog.h"

class Util
{
public:
	static ILog* getLogInstance();

private:
	static ILog* _sLogInstance;
};

#endif //__Util_H__