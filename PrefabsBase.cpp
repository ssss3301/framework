#include "PrefabsBase.h"

PrefabsBase::PrefabsBase()
{

}

PrefabsBase::~PrefabsBase()
{

}

bool PrefabsBase::init()
{
	return cocos2d::Node::init();
}

void PrefabsBase::awake()
{
	_childrenElements.clear();
	traversal(this);
}

void PrefabsBase::traversal(cocos2d::Node* node)
{
	cocos2d::Vector<cocos2d::Node*>& children = node->getChildren();
	if (children.size() <= 0)
		return;

	for (int i = 0; i < children.size(); ++i)
	{
		cocos2d::Node* child = children.at(i);
		if (child == nullptr)
			continue;

		std::string childName = child->getName();
		_childrenElements[childName] = child;

		traversal(child);
	}
}
