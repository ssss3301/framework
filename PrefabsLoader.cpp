#include "PrefabsLoader.h"
#include "cocostudio/CCSGUIReader.h"
#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "ui/UIHelper.h"
#include "ui/CocosGUI.h"
#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/WidgetReader/ProjectNodeReader/ProjectNodeReader.h"
#include "cocostudio/WidgetReader/ComAudioReader/ComAudioReader.h"
#include "base/ccMacros.h"
#include "cocostudio/WidgetCallBackHandlerProtocol.h"

static const char* ClassName_Node = "Node";
static const char* ClassName_SubGraph = "SubGraph";
static const char* ClassName_Sprite = "Sprite";
static const char* ClassName_Particle = "Particle";
static const char* ClassName_TMXTiledMap = "TMXTiledMap";

static const char* ClassName_Panel = "Panel";
static const char* ClassName_Button = "Button";
static const char* ClassName_CheckBox = "CheckBox";
static const char* ClassName_ImageView = "ImageView";
static const char* ClassName_TextAtlas = "TextAtlas";
static const char* ClassName_LabelAtlas = "LabelAtlas";
static const char* ClassName_LabelBMFont = "LabelBMFont";
static const char* ClassName_TextBMFont = "TextBMFont";
static const char* ClassName_Text = "Text";
static const char* ClassName_LoadingBar = "LoadingBar";
static const char* ClassName_TextField = "TextField";
static const char* ClassName_Slider = "Slider";
static const char* ClassName_Layout = "Layout";
static const char* ClassName_ScrollView = "ScrollView";
static const char* ClassName_ListView = "ListView";
static const char* ClassName_PageView = "PageView";
static const char* ClassName_Widget = "Widget";
static const char* ClassName_Label = "Label";

static const char* ClassName_ComAudio = "ComAudio";


static const char* NODE = "nodeTree";
static const char* CHILDREN = "children";
static const char* CLASSNAME = "classname";
static const char* FILE_PATH = "fileName";
static const char* PLIST_FILE = "plistFile";
static const char* TMX_FILE = "tmxFile";
static const char* TMX_STRING = "tmxString";
static const char* RESOURCE_PATH = "resourcePath";

static const char* COMPONENTS = "components";
static const char* COMPONENT_TYPE = "componentType";
static const char* COMPONENT_NAME = "componentName";
static const char* COMPONENT_ENABLED = "componentEnabled";
static const char* COMPONENT_AUDIO_FILE_PATH = "comAudioFilePath";
static const char* COMPONENT_LOOP = "comAudioloop";

static const char* TAG = "tag";
static const char* ACTION_TAG = "actionTag";

static const char* OPTIONS = "options";

static const char* WIDTH = "width";
static const char* HEIGHT = "height";
static const char* X = "x";
static const char* Y = "y";
static const char* SCALE_X = "scaleX";
static const char* SCALE_Y = "scaleY";
static const char* SKEW_X = "skewX";
static const char* SKEW_Y = "skewY";
static const char* ROTATION = "rotation";
static const char* ROTATION_SKEW_X = "rotationSkewX";
static const char* ROTATION_SKEW_Y = "rotationSkewY";
static const char* ANCHOR_X = "anchorPointX";
static const char* ANCHOR_Y = "anchorPointY";
static const char* ALPHA = "opacity";
static const char* RED = "colorR";
static const char* GREEN = "colorG";
static const char* BLUE = "colorB";
static const char* ZORDER = "ZOrder";
static const char* PARTICLE_NUM = "particleNum";
static const char* FLIPX = "flipX";
static const char* FLIPY = "flipY";
static const char* VISIBLE = "visible";

static const char* TEXTURES = "textures";
static const char* TEXTURES_PNG = "texturesPng";

static const char* MONO_COCOS2D_VERSION = "cocos2dVersion";


PrefabsLoader* PrefabsLoader::_sLoaderInstance = nullptr;

PrefabsLoader* PrefabsLoader::getInstance()
{
	if (_sLoaderInstance == nullptr)
	{
		_sLoaderInstance = new PrefabsLoader();
		_sLoaderInstance->init();
	}

	return _sLoaderInstance;
}

void PrefabsLoader::destroyInstance()
{
	CC_SAFE_DELETE(_sLoaderInstance);
}

PrefabsLoader::PrefabsLoader()
{

}

PrefabsBase* PrefabsLoader::createNode(const std::string& filename)
{
	std::string path = filename;
	size_t pos = path.find_last_of('.');
	std::string suffix = path.substr(pos + 1, path.length());

	PrefabsLoader* load = PrefabsLoader::getInstance();

	if (suffix == "csb")
	{
		return dynamic_cast<PrefabsBase*>(load->createNodeWithFlatBuffersFile(filename));
	}
	else if (suffix == "json" || suffix == "ExportJson")
	{
		return dynamic_cast<PrefabsBase*>(load->createNodeFromJson(filename));
	}

	return nullptr;
}

PrefabsBase* PrefabsLoader::createNode(const std::string& filename, const PrefabsLoadCallback& callback)
{
	std::string path = filename;
	size_t pos = path.find_last_of('.');
	std::string suffix = path.substr(pos + 1, path.length());

	PrefabsLoader* load = PrefabsLoader::getInstance();

	if (suffix == "csb")
	{
		return dynamic_cast<PrefabsBase*>(load->createNodeWithFlatBuffersFile(filename, callback));
	}

	return nullptr;;
}

PrefabsBase* PrefabsLoader::createNode(const cocos2d::Data data)
{
	return createNode(data, nullptr);
}

PrefabsBase* PrefabsLoader::createNode(const cocos2d::Data data, const cocos2d::ccNodeLoadCallback &callback)
{
	PrefabsLoader * loader = PrefabsLoader::getInstance();
	PrefabsBase * prefabs = nullptr;
	do
	{
		CC_BREAK_IF(data.isNull() || data.getSize() <= 0);
		auto csparsebinary = flatbuffers::GetCSParseBinary(data.getBytes());
		CC_BREAK_IF(nullptr == csparsebinary);
		auto csBuildId = csparsebinary->version();
		
		// decode plist
		auto textures = csparsebinary->textures();
		int textureSize = csparsebinary->textures()->size();
		CCLOG("textureSize = %d", textureSize);
		for (int i = 0; i < textureSize; ++i)
		{
			cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(textures->Get(i)->c_str());
		}

		prefabs = dynamic_cast<PrefabsBase*>(loader->nodeWithFlatBuffers(csparsebinary->nodeTree(), callback));
	} while (0);

	loader->reconstructNestNode(prefabs);

	return prefabs;

}

PrefabsBase* PrefabsLoader::createNodeWithVisibleSize(const std::string& filename)
{
	auto prefabs = createNode(filename);
	if (prefabs != nullptr)
	{
		cocos2d::Size frameSize = cocos2d::Director::getInstance()->getVisibleSize();
		prefabs->setContentSize(frameSize);
		cocos2d::ui::Helper::doLayout(prefabs);
	}
	return prefabs;
}

PrefabsBase* PrefabsLoader::createNodeWithVisibleSize(const std::string& filename, const cocos2d::ccNodeLoadCallback& callback)
{
	auto prefabs = createNode(filename, callback);
	if (prefabs != nullptr)
	{
		cocos2d::Size frameSize = cocos2d::Director::getInstance()->getVisibleSize();
		prefabs->setContentSize(frameSize);
		cocos2d::ui::Helper::doLayout(prefabs);
	}
	return prefabs;
}

cocostudio::timeline::ActionTimeline* PrefabsLoader::createTimeline(const std::string& filename)
{
	std::string suffix = getExtentionName(filename);

	cocostudio::timeline::ActionTimelineCache* cache = cocostudio::timeline::ActionTimelineCache::getInstance();

	if (suffix == "csb")
	{
		return cache->createActionWithFlatBuffersFile(filename);
	}
	else if (suffix == "json" || suffix == "ExportJson")
	{
		return cache->createActionFromJson(filename);
	}

	return nullptr;
}

cocostudio::timeline::ActionTimeline* PrefabsLoader::createTimeline(const cocos2d::Data data, const std::string& filename)
{
	std::string suffix = getExtentionName(filename);

	cocostudio::timeline::ActionTimelineCache* cache = cocostudio::timeline::ActionTimelineCache::getInstance();

	if (suffix == "csb")
	{
		return cache->loadAnimationWithDataBuffer(data, filename);
	}
	else if (suffix == "json" || suffix == "ExportJson")
	{
		std::string content((char *)data.getBytes(), data.getSize());
		return cache->loadAnimationActionWithContent(filename, content);
	}

	return nullptr;
}

PrefabsBase* PrefabsLoader::createNodeFromJson(const std::string& filename)
{
	if (_recordJsonPath)
	{
		std::string jsonPath = filename.substr(0, filename.find_last_of('/') + 1);
		cocostudio::GUIReader::getInstance()->setFilePath(jsonPath);

		_jsonPath = jsonPath;
	}
	else
	{
		cocostudio::GUIReader::getInstance()->setFilePath("");
		_jsonPath = "";
	}

	cocos2d::Node* node = loadNodeWithFile(filename);
	PrefabsBase* prefabs = dynamic_cast<PrefabsBase*>(node);
	if (prefabs)
		prefabs->awake();

	return prefabs;
}

cocos2d::Node* PrefabsLoader::loadNodeWithFile(const std::string& fileName)
{
	std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);

	cocos2d::Node* node = loadNodeWithContent(contentStr);

	// Load animation data from file
	cocostudio::timeline::ActionTimelineCache::getInstance()->loadAnimationActionWithContent(fileName, contentStr);

	return node;
}

cocos2d::Node* PrefabsLoader::loadNodeWithContent(const std::string& content)
{
	rapidjson::Document doc;
	doc.Parse<0>(content.c_str());
	if (doc.HasParseError())
	{
		CCLOG("GetParseError %d\n", doc.GetParseError());
	}

	// cocos2dx version mono editor is based on
	_monoCocos2dxVersion = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(doc, MONO_COCOS2D_VERSION, "");

	// decode plist
	int length = cocostudio::DictionaryHelper::getInstance()->getArrayCount_json(doc, TEXTURES);

	for (int i = 0; i < length; i++)
	{
		std::string plist = cocostudio::DictionaryHelper::getInstance()->getStringValueFromArray_json(doc, TEXTURES, i);
		std::string png = cocostudio::DictionaryHelper::getInstance()->getStringValueFromArray_json(doc, TEXTURES_PNG, i);
		plist = _jsonPath + plist;
		png = _jsonPath + png;
		cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, png);
	}

	// decode node tree
	const rapidjson::Value& subJson = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(doc, NODE);
	cocos2d::Node* node = loadNode(subJson);
	node->release();

	return node;
}

PrefabsBase* PrefabsLoader::createNodeWithFlatBuffersFile(const std::string& filename)
{
	return createNodeWithFlatBuffersFile(filename, nullptr);
}

cocos2d::Node* PrefabsLoader::nodeWithFlatBuffersFile(const std::string& fileName)
{
	return nodeWithFlatBuffersFile(fileName, nullptr);
}

cocos2d::Node* PrefabsLoader::nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree)
{
	return nodeWithFlatBuffers(nodetree, nullptr);
}

cocos2d::Node* PrefabsLoader::createNodeWithFlatBuffersForSimulator(const std::string& filename)
{
	cocostudio::FlatBuffersSerialize* fbs = cocostudio::FlatBuffersSerialize::getInstance();
	fbs->_isSimulator = true;
	flatbuffers::FlatBufferBuilder* builder = fbs->createFlatBuffersWithXMLFileForSimulator(filename);

	auto csparsebinary = flatbuffers::GetCSParseBinary(builder->GetBufferPointer());

	// decode plist
	auto textures = csparsebinary->textures();
	int textureSize = csparsebinary->textures()->size();
	//    CCLOG("textureSize = %d", textureSize);
	for (int i = 0; i < textureSize; ++i)
	{
		cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(textures->Get(i)->c_str());
	}

	auto nodeTree = csparsebinary->nodeTree();

	cocos2d::Node* node = nodeWithFlatBuffersForSimulator(nodeTree);

	_rootNode = nullptr;

	fbs->deleteFlatBufferBuilder();

	return node;
}

cocos2d::Node* PrefabsLoader::nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree)
{
	cocos2d::Node* node = nullptr;

	std::string classname = nodetree->classname()->c_str();

	auto options = nodetree->options();

	if (classname == "ProjectNode")
	{
		auto reader = cocostudio::ProjectNodeReader::getInstance();
		auto projectNodeOptions = (flatbuffers::ProjectNodeOptions*)options->data();
		std::string filePath = projectNodeOptions->fileName()->c_str();

		cocostudio::timeline::ActionTimeline* action = nullptr;
		if (filePath != "" && cocos2d::FileUtils::getInstance()->isFileExist(filePath))
		{
			node = createNodeWithFlatBuffersForSimulator(filePath);
			action = cocostudio::timeline::ActionTimelineCache::getInstance()->createActionWithFlatBuffersForSimulator(filePath);
		}
		else
		{
			node = cocos2d::Node::create();
		}
		reader->setPropsWithFlatBuffers(node, options->data());
		if (action)
		{
			action->setTimeSpeed(projectNodeOptions->innerActionSpeed());
			node->runAction(action);
			action->gotoFrameAndPause(0);
		}
	}
	else if (classname == "SimpleAudio")
	{
		node = cocos2d::Node::create();
		auto reader = cocostudio::ComAudioReader::getInstance();
		cocos2d::Component* component = reader->createComAudioWithFlatBuffers(options->data());
		if (component)
		{
			node->addComponent(component);
			reader->setPropsWithFlatBuffers(node, options->data());
		}
	}
	else
	{
		std::string readername = getGUIClassName(classname);
		readername.append("Reader");

		cocostudio::NodeReaderProtocol* reader = dynamic_cast<cocostudio::NodeReaderProtocol*>(cocos2d::ObjectFactory::getInstance()->createObject(readername));
		if (reader)
		{
			node = reader->createNodeWithFlatBuffers(options->data());
		}

		cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(node);
		if (widget)
		{
			std::string callbackName = widget->getCallbackName();
			std::string callbackType = widget->getCallbackType();

			bindCallback(callbackName, callbackType, widget, _rootNode);
		}

		if (_rootNode == nullptr)
		{
			_rootNode = node;
		}
	}

	// If node is invalid, there is no necessity to process children of node.
	if (!node)
	{
		return nullptr;
	}

	auto children = nodetree->children();
	int size = children->size();
	for (int i = 0; i < size; ++i)
	{
		auto subNodeTree = children->Get(i);
		cocos2d::Node* child = nodeWithFlatBuffersForSimulator(subNodeTree);
		if (child)
		{
			cocos2d::ui::PageView* pageView = dynamic_cast<cocos2d::ui::PageView*>(node);
			cocos2d::ui::ListView* listView = dynamic_cast<cocos2d::ui::ListView*>(node);
			if (pageView)
			{
				cocos2d::ui::Layout* layout = dynamic_cast<cocos2d::ui::Layout*>(child);
				if (layout)
				{
					pageView->addPage(layout);
				}
			}
			else if (listView)
			{
				cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(child);
				if (widget)
				{
					listView->pushBackCustomItem(widget);
				}
			}
			else
			{
				node->addChild(child);
			}
		}
	}

	return node;
}

PrefabsBase* PrefabsLoader::createNodeWithFlatBuffersFile(const std::string& filename, const PrefabsLoadCallback& callback)
{
	cocos2d::Node* node = nodeWithFlatBuffersFile(filename, callback);
	reconstructNestNode(node);

	PrefabsBase* prefabs = dynamic_cast<PrefabsBase*>(node);
	if (prefabs)
		prefabs->awake();

	return prefabs;
}

cocos2d::Node* PrefabsLoader::nodeWithFlatBuffersFile(const std::string& fileName, const PrefabsLoadCallback& callback)
{
	std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);

	CC_ASSERT(cocos2d::FileUtils::getInstance()->isFileExist(fullPath));

	cocos2d::Data buf = cocos2d::FileUtils::getInstance()->getDataFromFile(fullPath);

	if (buf.isNull())
	{
		CCLOG("CSLoader::nodeWithFlatBuffersFile - failed read file: %s", fileName.c_str());
		CC_ASSERT(false);
		return nullptr;
	}

	auto csparsebinary = flatbuffers::GetCSParseBinary(buf.getBytes());

	auto csBuildId = csparsebinary->version();
	if (csBuildId)
	{
		CCASSERT(strcmp(_csBuildID.c_str(), csBuildId->c_str()) == 0,
			cocos2d::StringUtils::format("%s%s%s%s%s%s%s%s%s%s",
			"The reader build id of your Cocos exported file(",
			csBuildId->c_str(),
			") and the reader build id in your Cocos2d-x(",
			_csBuildID.c_str(),
			") are not match.\n",
			"Please get the correct reader(build id ",
			csBuildId->c_str(),
			")from ",
			"http://www.cocos2d-x.org/filedown/cocos-reader",
			" and replace it in your Cocos2d-x").c_str());
	}

	// decode plist
	auto textures = csparsebinary->textures();
	int textureSize = textures->size();
	for (int i = 0; i < textureSize; ++i)
	{
		cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(textures->Get(i)->c_str());
	}

	cocos2d::Node* node = nodeWithFlatBuffers(csparsebinary->nodeTree(), callback);

	return node;

}

cocos2d::Node* PrefabsLoader::nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree, const PrefabsLoadCallback& callback)
{
	if (nodetree == nullptr)
		return nullptr;

	{
		cocos2d::Node* node = nullptr;

		std::string classname = nodetree->classname()->c_str();

		auto options = nodetree->options();

		if (classname == "ProjectNode")
		{
			auto reader = cocostudio::ProjectNodeReader::getInstance();
			auto projectNodeOptions = (flatbuffers::ProjectNodeOptions*)options->data();
			std::string filePath = projectNodeOptions->fileName()->c_str();

			cocostudio::timeline::ActionTimeline* action = nullptr;
			if (filePath != "" && cocos2d::FileUtils::getInstance()->isFileExist(filePath))
			{
				cocos2d::Data buf = cocos2d::FileUtils::getInstance()->getDataFromFile(filePath);
				node = createNode(buf, callback);
				action = cocostudio::timeline::ActionTimelineCache::getInstance()->loadAnimationWithDataBuffer(buf, filePath);
			}
			else
			{
				node = cocos2d::Node::create();
			}
			reader->setPropsWithFlatBuffers(node, options->data());
			if (action)
			{
				action->setTimeSpeed(projectNodeOptions->innerActionSpeed());
				node->runAction(action);
				action->gotoFrameAndPause(0);
			}
		}
		else if (classname == "SimpleAudio")
		{
			node = cocos2d::Node::create();
			auto reader = cocostudio::ComAudioReader::getInstance();
			cocos2d::Component* component = reader->createComAudioWithFlatBuffers(options->data());
			if (component)
			{
				node->addComponent(component);
				reader->setPropsWithFlatBuffers(node, options->data());
			}
		}
		else
		{
			std::string customClassName = nodetree->customClassName()->c_str();
			if (customClassName != "")
			{
				classname = customClassName;
			}
			std::string readername = getGUIClassName(classname);
			readername.append("Reader");

			cocostudio::NodeReaderProtocol* reader = dynamic_cast<cocostudio::NodeReaderProtocol*>(cocos2d::ObjectFactory::getInstance()->createObject(readername));
			if (reader)
			{
				node = reader->createNodeWithFlatBuffers(options->data());
			}

			cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(node);
			if (widget)
			{
				std::string callbackName = widget->getCallbackName();
				std::string callbackType = widget->getCallbackType();

				bindCallback(callbackName, callbackType, widget, _rootNode);
			}

			/* To reconstruct nest node as WidgetCallBackHandlerProtocol. */
			auto callbackHandler = dynamic_cast<cocostudio::WidgetCallBackHandlerProtocol*>(node);
			if (callbackHandler)
			{
				_callbackHandlers.pushBack(node);
				_rootNode = _callbackHandlers.back();
			}
			
		}

		// If node is invalid, there is no necessity to process children of node.
		if (!node)
		{
			return nullptr;
		}

		std::string readername = getGUIClassName(classname);
		auto children = nodetree->children();
		int size = children->size();
		for (int i = 0; i < size; ++i)
		{
			auto subNodeTree = children->Get(i);
			cocos2d::Node* child = nodeWithFlatBuffers(subNodeTree, callback);
			if (child)
			{
				cocos2d::ui::PageView* pageView = dynamic_cast<cocos2d::ui::PageView*>(node);
				cocos2d::ui::ListView* listView = dynamic_cast<cocos2d::ui::ListView*>(node);
				if (pageView)
				{
					cocos2d::ui::Layout* layout = dynamic_cast<cocos2d::ui::Layout*>(child);
					if (layout)
					{
						pageView->addPage(layout);
					}
				}
				else if (listView)
				{
					cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(child);
					if (widget)
					{
						listView->pushBackCustomItem(widget);
					}
				}
				else
				{
					node->addChild(child);
				}

				if (callback)
				{
					callback(child);
				}
			}
		}

		return node;
	}
}

cocos2d::Node* PrefabsLoader::loadNode(const rapidjson::Value& json)
{
	cocos2d::Node* node = nullptr;
	std::string nodeType = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, CLASSNAME);

	NodeCreateFunc func = _funcs[nodeType];
	if (func != nullptr)
	{
		const rapidjson::Value& options = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, OPTIONS);
		node = func(options);

		// component
		if (node)
		{
			const rapidjson::Value& components = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(options, COMPONENTS);
			int componentSize = cocostudio::DictionaryHelper::getInstance()->getArrayCount_json(options, COMPONENTS, 0);
			for (int i = 0; i < componentSize; ++i)
			{
				const rapidjson::Value &dic = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(components, COMPONENTS, i);
				cocos2d::Component* component = loadComponent(dic);
				if (component)
				{
					node->addComponent(component);
				}
			}
		}
	}

	if (node)
	{
		int length = cocostudio::DictionaryHelper::getInstance()->getArrayCount_json(json, CHILDREN, 0);
		for (int i = 0; i < length; i++)
		{
			const rapidjson::Value &dic = cocostudio::DictionaryHelper::getInstance()->getSubDictionary_json(json, CHILDREN, i);
			cocos2d::Node* child = loadNode(dic);
			if (child)
			{
				cocos2d::ui::PageView* pageView = dynamic_cast<cocos2d::ui::PageView*>(node);
				cocos2d::ui::ListView* listView = dynamic_cast<cocos2d::ui::ListView*>(node);
				if (pageView)
				{
					cocos2d::ui::Layout* layout = dynamic_cast<cocos2d::ui::Layout*>(child);
					if (layout)
					{
						pageView->addPage(layout);
					}
				}
				else if (listView)
				{
					cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(child);
					if (widget)
					{
						listView->pushBackCustomItem(widget);
					}
				}
				else
				{
					if (_monoCocos2dxVersion != "3.x")
					{
						cocos2d::ui::Widget* widget = dynamic_cast<cocos2d::ui::Widget*>(child);
						cocos2d::ui::Widget* parent = dynamic_cast<cocos2d::ui::Widget*>(node);
						if (widget
							&& parent
							&& !dynamic_cast<cocos2d::ui::Layout*>(parent))
						{
							if (widget->getPositionType() == cocos2d::ui::Widget::PositionType::PERCENT)
							{
								widget->setPositionPercent(cocos2d::Vec2(widget->getPositionPercent().x + parent->getAnchorPoint().x, widget->getPositionPercent().y + parent->getAnchorPoint().y));
								widget->setPosition(cocos2d::Vec2(widget->getPositionX() + parent->getAnchorPointInPoints().x, widget->getPositionY() + parent->getAnchorPointInPoints().y));
							}
							else
							{
								cocos2d::Size parentSize = parent->getContentSize();
								widget->setPosition(cocos2d::Vec2(widget->getPositionX() + parentSize.width * parent->getAnchorPoint().x,
									widget->getPositionY() + parentSize.height * parent->getAnchorPoint().y));
							}
						}
					}

					node->addChild(child);
				}
				child->release();
			}
		}
	}
	else
	{
		CCLOG("Not supported NodeType: %s", nodeType.c_str());
	}

	return node;
}

cocos2d::Node* PrefabsLoader::loadSimpleNode(const rapidjson::Value& json)
{
	PrefabsBase* prefabs = PrefabsBase::create();
	initNode(prefabs, json);
	return prefabs;
}

cocos2d::Node* PrefabsLoader::loadSubGraph(const rapidjson::Value& json)
{
	const char* filePath = cocostudio::DictionaryHelper::getInstance()->getStringValue_json(json, FILE_PATH);

	PrefabsBase* prefabs = nullptr;
	if (filePath && strcmp("", filePath) != 0)
	{
		prefabs = createNode(filePath);
	}
	else
	{
		prefabs = PrefabsBase::create();
	}
	initNode(prefabs, json);
	return prefabs;
}