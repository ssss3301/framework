#ifndef __ILog_H__
#define __ILog_H__

class ILog
{
public:
	ILog(){	}
	virtual ~ILog() {}
	virtual void Log(char* fmt, ...) = 0;

protected:
	const static int MAXLOGLENGTH = 10 * 1024;
};

#endif //__ILog_H__