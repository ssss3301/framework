#include "Util.h"
#include "ILogCocos2dx.h"

ILog* Util::_sLogInstance = nullptr;

ILog* Util::getLogInstance()
{
	if (_sLogInstance == nullptr)
		_sLogInstance = new ILogCocos2dx();

	return _sLogInstance;
}

