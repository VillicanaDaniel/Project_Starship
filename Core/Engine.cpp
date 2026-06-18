#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

Engine* g_engine = nullptr;

Engine::Engine(EngineConfig const& config)
	:m_config(config)
{
	g_engine = this;

	m_event = new EventSystem(config.m_eventConfig);
	m_window = new Window(config.m_windowConfig);
	m_render = new Renderer(config.m_rendererConfig);
	m_devConsole = new DevConsole(config.m_devConsoleConfig);
	m_input = new InputSystem(config.m_inputConfig);
	m_audio = new AudioSystem(config.m_audioConfig);
	
	m_event->Startup();
	m_window->Startup();
	m_render->Startup();
	m_devConsole->Startup();
	m_input->Startup();
	m_audio->Startup();
}


Engine::~Engine()
{
	m_audio->Shutdown();
	delete m_audio;
	m_audio = nullptr;

	m_input->Shutdown();
	delete m_input;
	m_input = nullptr;

	m_devConsole->Shutdown();
	delete m_devConsole;
	m_devConsole = nullptr;

	m_render->Shutdown();
	delete m_render;
	m_render = nullptr;

	m_window->Shutdown();
	delete m_window;
	m_window = nullptr;

	m_event->Shutdown();
	delete m_event;
	m_event = nullptr;
}


void Engine::BeginFrame()
{
	Clock::TickSystemClock();

	m_event->BeginFrame();
	m_window->BeginFrame();
	m_audio->BeginFrame();
	m_input->BeginFrame();
	m_devConsole->BeginFrame();
	m_render->BeginFrame();
}


void Engine::EndFrame()
{
	m_render->EndFrame();
	m_devConsole->EndFrame();
	m_input->EndFrame();
	m_audio->EndFrame();
	m_window->EndFrame();
	m_event->EndFrame();
}