#ifndef __PrefabsLoader_H__
#define __PrefabsLoader_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "PrefabsBase.h"

typedef std::function<void(cocos2d::Ref*)> PrefabsLoadCallback;

class PrefabsLoader : public cocos2d::CSLoader
{
public:
	static PrefabsLoader* getInstance();
	static void destroyInstance();

	PrefabsLoader();

	static PrefabsBase* createNode(const std::string& filename);
	static PrefabsBase* createNode(const std::string& filename, const PrefabsLoadCallback& callback);
	static PrefabsBase* createNode(const cocos2d::Data data);
	static PrefabsBase* createNode(const cocos2d::Data data, const PrefabsLoadCallback &callback);
	static PrefabsBase* createNodeWithVisibleSize(const std::string& filename);
	static PrefabsBase* createNodeWithVisibleSize(const std::string& filename, const cocos2d::ccNodeLoadCallback& callback);

	static cocostudio::timeline::ActionTimeline* createTimeline(const std::string& filename);
	static cocostudio::timeline::ActionTimeline* createTimeline(const cocos2d::Data data, const std::string& filename);

	PrefabsBase* createNodeFromJson(const std::string& filename);
	cocos2d::Node* loadNodeWithFile(const std::string& fileName);
	cocos2d::Node* loadNodeWithContent(const std::string& content);

	PrefabsBase* createNodeWithFlatBuffersFile(const std::string& filename);
	cocos2d::Node* nodeWithFlatBuffersFile(const std::string& fileName);
	cocos2d::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree);

	cocos2d::Node* createNodeWithFlatBuffersForSimulator(const std::string& filename);
	cocos2d::Node* nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree);

protected:
	PrefabsBase* createNodeWithFlatBuffersFile(const std::string& filename, const PrefabsLoadCallback& callback);
	cocos2d::Node* nodeWithFlatBuffersFile(const std::string& fileName, const PrefabsLoadCallback& callback);
	cocos2d::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree, const PrefabsLoadCallback& callback);
	cocos2d::Node* loadNode(const rapidjson::Value& json);
	cocos2d::Node* loadSimpleNode(const rapidjson::Value& json);
	cocos2d::Node* loadSubGraph(const rapidjson::Value& json);

private:
	static PrefabsLoader* _sLoaderInstance;
};

#endif //__PrefabsLoader_H__