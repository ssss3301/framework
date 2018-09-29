#ifndef __UIMaskManager_H__
#define __UIMaskManager_H__

#include "UIMask.h"
#include <queue>

class UIMaskManager
{
public:
	static UIMaskManager* getInstance();
	UIMaskManager();
	virtual ~UIMaskManager();

	void initWithCapacity(int capacity);

	void pushMask(UIMask* mask);
	UIMask* popMask();

private:
	static UIMaskManager* _maskManagerInstance;
	std::queue<UIMask*>	_maskQueue;
};

#endif //__UIMaskManager_H__