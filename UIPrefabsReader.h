#ifndef __UIPrefabsReader_H__
#define __UIPrefabsReader_H__

#include "cocos2d.h"
#include "PrefabsReader.h"
#include "PrefabsBase.h"

template <class T>
class UIPrefabsReader : public PrefabsReader
{
	static_assert((std::is_base_of<PrefabsBase, T>::value), "T must inherit from PrefabsBase");
public:
	UIPrefabsReader() {}
	~UIPrefabsReader() {}

	static UIPrefabsReader* getInstance()
	{
		if (_sInstance == nullptr)
			_sInstance = new UIPrefabsReader();

		return _sInstance;
	}

	static void destroyInstance()
	{
		CC_SAFE_DELETE(_sInstance);
	}

	PrefabsBase* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
	{
		PrefabsBase* prefabs = (PrefabsBase*)new T();
		if (prefabs == nullptr)
			return nullptr;

		if (!prefabs->init())
		{
			CC_SAFE_DELETE(prefabs);
			return nullptr;
		}
		prefabs->autorelease();
		setPropsWithFlatBuffers(prefabs, nodeOptions);
		return prefabs;
	}

private:
	static UIPrefabsReader* _sInstance;
};

template<class T> UIPrefabsReader<T>* UIPrefabsReader<T>::_sInstance = nullptr;

#endif //__UIPrefabReader_H__