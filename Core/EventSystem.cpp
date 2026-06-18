#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"

EventSystem::EventSystem(EventSystemConfig const& config)
	: m_config(config)
{
}

void EventSystem::Startup()
{
	//TODO?
}

void EventSystem::Shutdown()
{
	m_listOfSubscribersByEventName.clear();
}

void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction)
{
	m_listOfSubscribersByEventName[eventName].push_back(callbackFunction);
}

void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction)
{
	SubscriberList& subscribers = m_listOfSubscribersByEventName[eventName];

	for (int i = 0; i < (int)subscribers.size(); ++i)
	{
		if (subscribers[i] == callbackFunction)
		{
			subscribers[i] = nullptr;
			break;
		}
	}
}

int EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	int numRecipients = 0;
	SubscriberList& subscribers = m_listOfSubscribersByEventName[eventName];

	for (int i = 0; i < subscribers.size(); ++i)
	{
		if (subscribers[i] != nullptr)
		{
			bool wasConsumed = subscribers[i](args);
			++numRecipients;
			if (wasConsumed)
			{
				break; //dont't call any other callbacks; event was consumed by this recipient;
			}
		}
	}

	return numRecipients;
}

int EventSystem::FireEvent(std::string const& eventName)
{
	EventArgs args;
	return FireEvent(eventName, args);
}

void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction)
{
	if (g_engine && g_engine->m_event)
	{
		g_engine->m_event->SubscribeEventCallbackFunction(eventName, callbackFunction);
	}
}

void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction)
{
	if (g_engine && g_engine->m_event)
	{
		g_engine->m_event->UnsubscribeEventCallbackFunction(eventName, callbackFunction);
	}
}

int FireEvent(std::string const& eventName, EventArgs& args)
{
	if (g_engine && g_engine->m_event)
	{
		return g_engine->m_event->FireEvent(eventName, args);
	}
	return 0;
}

int FireEvent(std::string const& eventName)
{
	if (g_engine && g_engine->m_event)
	{
		return g_engine->m_event->FireEvent(eventName);
	}
	return 0;
}

