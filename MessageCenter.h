#ifndef __MessageCenter_H__
#define __MessageCenter_H__

#include "GlobalDefine.h"
#include <vector>
#include <map>

class MessageListener
{
public:
	MessageListener();
	MessageListener(std::function<void(MessageID, void*)> func);
	virtual ~MessageListener();

	unsigned long id() const;
	bool operator==(const MessageListener& listener);

public:
	std::function<void(MessageID, void*)> _func;

private:
	static unsigned long _id;
};

class MessageCenter
{
	typedef std::vector<MessageListener> MessageListenerList;
	typedef std::map<MessageID, MessageListenerList> MessageIDListenerListKV;
	typedef std::map<MessageType, MessageIDListenerListKV> MessageTypeListenerListKV;

public:
	static MessageCenter* getInstance();
	MessageCenter();
	virtual ~MessageCenter();

	void addListener(MessageType msgType, MessageID msgID, MessageListener listener);
	void removeListener(MessageType msgType, MessageID msgID, MessageListener listener);

	void sendMessage(MessageType msgType, MessageID msgID, void* data);

private:
	bool tryGetMessageIDListenerListKV(MessageType msgType);
	bool tryGetMessageListenerList(MessageIDListenerListKV& kv, MessageID msgID);
	bool tryGetMessageListenerList(MessageType msgType, MessageID msgID);

	void removeListenerFromList(MessageListenerList& listenerList, MessageListener listener);
	MessageListenerList& getMessageListenerList(MessageType msgType, MessageID msgID);

private:
	static MessageCenter*		_sInstance;
	MessageTypeListenerListKV	_msgListeners;
};	

#endif //__MessageCenter_H__