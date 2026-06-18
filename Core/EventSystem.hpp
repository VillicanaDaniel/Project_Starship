#pragma once
#include "Engine/Core/NamedStrings.hpp"

typedef NamedStrings EventArgs;
typedef bool(*EventSystemCallbackFunctionPtr)(EventArgs& args);
typedef std::vector<EventSystemCallbackFunctionPtr> SubscriberList;

struct EventSystemConfig
{
	bool	m_isEnabled = true;
};

class EventSystem
{
public:
	EventSystem(EventSystemConfig const& config);
	~EventSystem() = default;
	void Startup();
	void Shutdown();

	void BeginFrame() {};
	void EndFrame() {};

	void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction);
	int FireEvent(std::string const& eventName, EventArgs& args);
	int FireEvent(std::string const& eventName);

protected:
	EventSystemConfig m_config;
	std::map< std::string, SubscriberList > m_listOfSubscribersByEventName;
};

void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunctionPtr callbackFunction);
int FireEvent(std::string const& eventName, EventArgs& args);
int FireEvent(std::string const& eventName);