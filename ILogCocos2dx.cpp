#include "ILogCocos2dx.h"
#include "cocos2d.h"


ILogCocos2dx::ILogCocos2dx()
{

}

ILogCocos2dx::~ILogCocos2dx()
{

}

void ILogCocos2dx::Log(char* fmt, ...)
{
	char* buffer = new char[MAXLOGLENGTH];
	if (buffer == nullptr)
		return;

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);

	cocos2d::log(buffer);
	delete[] buffer;
}
