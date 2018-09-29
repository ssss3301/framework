#include "PrefabsReader.h"
#include "PrefabsBase.h"

static PrefabsReader* _sInstanceReader = nullptr;

PrefabsReader* PrefabsReader::getInstance()
{
	if (_sInstanceReader == nullptr)
		_sInstanceReader = new PrefabsReader();
	return _sInstanceReader;
}

void PrefabsReader::destoryInstance()
{
	CC_SAFE_DELETE(_sInstanceReader);
}

cocos2d::Node* PrefabsReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
	PrefabsBase* prefabs = PrefabsBase::create();
	setPropsWithFlatBuffers(prefabs, nodeOptions);
	return prefabs;
}
