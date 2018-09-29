#ifndef __PrefabsReader_H__
#define __PrefabsReader_H__

#include "cocos2d.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

class PrefabsReader : public cocostudio::NodeReader
{
public:
	static PrefabsReader* getInstance();
	static void destoryInstance();
	virtual cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions);
};

#endif //__PrefabsReader_H__