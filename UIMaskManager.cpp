#include "UIMaskManager.h"

const static int default_mask_capacity = 5;
UIMaskManager* UIMaskManager::_maskManagerInstance = nullptr;

UIMaskManager* UIMaskManager::getInstance()
{
	if (_maskManagerInstance == nullptr)
		_maskManagerInstance = new UIMaskManager();

	return _maskManagerInstance;
}

UIMaskManager::UIMaskManager()
{
	initWithCapacity(default_mask_capacity);
}

UIMaskManager::~UIMaskManager()
{
	while (_maskQueue.size() > 0)
	{
		UIMask* mask = _maskQueue.front();
		if (mask)
			mask->release();
		_maskQueue.pop();
	}
}

void UIMaskManager::initWithCapacity(int capacity)
{
	for (int i = 0; i < capacity; ++i)
	{
		UIMask* mask = UIMask::create();
		if (mask)
		{
			mask->retain();
			_maskQueue.push(mask);
		}
	}
}

void UIMaskManager::pushMask(UIMask* mask)
{
	if (mask == nullptr)
		return;
	_maskQueue.push(mask);
	mask->retain();
}

UIMask* UIMaskManager::popMask()
{
	UIMask* mask = nullptr;
	if (_maskQueue.size() > 0)
	{
		mask = _maskQueue.front();
		if (mask)
			mask->release();
		_maskQueue.pop();
	}
	else
	{
		mask = UIMask::create();
	}

	return mask;
}
