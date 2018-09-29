#ifndef __UIPrefabsBase_H__
#define __UIPrefabsBase_H__

#include "cocos2d.h"
#include <map>
#include <string>

class PrefabsBase : public cocos2d::Node
{
public:
	CREATE_FUNC(PrefabsBase);
	PrefabsBase();
	virtual ~PrefabsBase();
	virtual bool init();
	virtual void awake();

private:
	void traversal(cocos2d::Node* node);

protected:
	std::map<std::string, cocos2d::Node*>	_childrenElements;
};

#endif //__UIPrefabsBase_H__