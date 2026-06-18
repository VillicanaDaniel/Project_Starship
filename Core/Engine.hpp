#pragma once

#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"

class Engine;

extern Engine* g_engine;

struct EngineConfig
{
	AudioConfig m_audioConfig;
	InputConfig	m_inputConfig;
	WindowConfig m_windowConfig;
	RendererConfig m_rendererConfig;
	DevConsoleConfig m_devConsoleConfig;
	EventSystemConfig m_eventConfig;
};

class Engine
{
	public:
		Engine(EngineConfig const& config);
		~Engine();
		void BeginFrame();
		void EndFrame();

	public:
		Window* m_window = nullptr;
		Renderer* m_render = nullptr;
		InputSystem* m_input = nullptr;
		AudioSystem* m_audio = nullptr;
		DevConsole* m_devConsole = nullptr;
		EventSystem* m_event = nullptr;

		EngineConfig m_config;
};