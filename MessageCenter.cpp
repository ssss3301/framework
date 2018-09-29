#include "MessageCenter.h"

unsigned long MessageListener::_id = 0;
MessageCenter*	MessageCenter::_sInstance = nullptr;

MessageListener::MessageListener()
{
	_id++;
}

MessageListener::MessageListener(std::function<void(MessageID, void*)> func)
{
	_func = func;
	_id++;
}

MessageListener::~MessageListener()
{

}

unsigned long MessageListener::id() const
{
	return _id;
}

bool MessageListener::operator==(const MessageListener& listener)
{
	return (_id == listener.id());
}

MessageCenter* MessageCenter::getInstance()
{
	if (_sInstance == nullptr)
		_sInstance = new MessageCenter();
	
	return _sInstance;
}

MessageCenter::MessageCenter()
{

}

MessageCenter::~MessageCenter()
{

}

void MessageCenter::addListener(MessageType msgType, MessageID msgID, MessageListener listener)
{
	MessageIDListenerListKV& kv = _msgListeners[msgType];
	kv[msgID].push_back(listener);
}

void MessageCenter::removeListener(MessageType msgType, MessageID msgID, MessageListener listener)
{
	if (!tryGetMessageIDListenerListKV(msgType))
		return;

	MessageIDListenerListKV& kv = _msgListeners[msgType];
	if (!tryGetMessageListenerList(kv, msgID))
		return;

	MessageListenerList& listenerList = kv[msgID];
	removeListenerFromList(listenerList, listener);
}

void MessageCenter::sendMessage(MessageType msgType, MessageID msgID, void* data)
{
	if (!tryGetMessageListenerList(msgType, msgID))
		return;

	MessageListenerList& listenerList = getMessageListenerList(msgType, msgID);
	for (int i = 0; i < listenerList.size(); ++i)
	{
		MessageListener& listener = listenerList[i];
		if (listener._func)
			listener._func(msgID, data);
	}
}

bool MessageCenter::tryGetMessageIDListenerListKV(MessageType msgType)
{
	MessageTypeListenerListKV::iterator iter = _msgListeners.find(msgType);
	if (iter != _msgListeners.end())
		return true;

	return false;
}

bool MessageCenter::tryGetMessageListenerList(MessageIDListenerListKV& kv, MessageID msgID)
{
	MessageIDListenerListKV::iterator iter = kv.find(msgID);
	if (iter != kv.end())
		return true;

	return false;
}

bool MessageCenter::tryGetMessageListenerList(MessageType msgType, MessageID msgID)
{
	if (!tryGetMessageIDListenerListKV(msgType))
		return false;

	if (!tryGetMessageListenerList(_msgListeners[msgType], msgID))
		return false;

	return true;
}

void MessageCenter::removeListenerFromList(MessageListenerList& listenerList, MessageListener listener)
{
	MessageListenerList::iterator iter = listenerList.begin();
	for (; iter != listenerList.end(); ++iter)
	{
		if (*iter == listener)
		{
			listenerList.erase(iter);
			break;
		}
	}
}

MessageCenter::MessageListenerList& MessageCenter::getMessageListenerList(MessageType msgType, MessageID msgID)
{
	MessageIDListenerListKV& kv = _msgListeners[msgType];
	return kv[msgID];
}


