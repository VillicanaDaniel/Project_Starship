#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


App* g_theApp = nullptr;

App::App()
{
	g_theApp = this;
	EngineConfig config;
	config.m_windowConfig.m_clientAspect = 2.f;
	g_engine = new Engine( config );

	m_systemClock = new Clock();
	m_systemClock->SetTimeScale(1.0);

	g_engine->m_event->SubscribeEventCallbackFunction("Quit", &App::SetIsQuittingEvent);
	g_engine->m_event->SubscribeEventCallbackFunction("set_game_time_scale", &App::Command_SetGameTimeScale);
	g_engine->m_event->SubscribeEventCallbackFunction("KeyPressed", &App::OnKeyDownEvent);

	m_game = new Game();
	m_gameCamera = m_game->m_screenCamera;
}

App::~App()
{
	delete g_engine;
	g_engine = nullptr;

	delete m_game;
	m_game = nullptr;
}

void App::RunFrame()
{
	m_systemClock->Tick();
	// 	float timeNow = (float) GetCurrentTimeSeconds();
	// 	float deltaSeconds = timeNow - m_lastFrameTime;
	// 	m_lastFrameTime = timeNow;

	g_engine->BeginFrame();
	Update();
	Render();		// #SD1ToDo: ...becomes just Render();		once this function becomes App::Run()
	g_engine->EndFrame(); // Allow engine subsystems to do post-frame stuff

}


void App::Update()
{
	InputSystem* input = g_engine->m_input;
	UpdateFromKeyboard();
	for (int id = 0; id < 4; ++id)
	{
		input->GetController(id);
	}

	if (m_stepSingleFrameRequested && m_isPaused)
	{
		m_stepSingleFrameRequested = false;
		m_isPaused = false;
		m_pauseAfterNextUpdate = true;

		float restore = (m_timeScaleBeforePause > 0.0f) ? m_timeScaleBeforePause : 1.0f;
		m_game->GetClock()->SetTimeScale(restore);
	}

	if (m_isPaused)
	{
		m_game->GetClock()->SetTimeScale(0.0f);
	}

	m_game->Update();

	if (m_pauseAfterNextUpdate)
	{
		m_pauseAfterNextUpdate = false;
		m_isPaused = true;
		m_game->GetClock()->SetTimeScale(0.0f);
	}

	//Cases for pause and slow, will be changed later
// 	if (m_isPaused == true)
// 		m_game->GetClock()->SetTimeScale(0.f);
// 	else if (m_isSlowMo == true)
// 		m_game->GetClock()->SetTimeScale(0.1f);
// 	else if (m_isFastMo == true)
// 		m_game->GetClock()->SetTimeScale(4.f);
// 	else if (m_isPaused == false)
// 		m_game->GetClock()->SetTimeScale(1.f);
// 	else if (m_isSlowMo == false)
// 		m_game->GetClock()->SetTimeScale(1.f);
// 	else if (m_isFastMo == false)
// 		m_game->GetClock()->SetTimeScale(1.f);
// 
// 	if (m_pauseAfterNextUpdate)
// 	{
// 		m_game->GetClock()->SetTimeScale(1.f);
// 		m_pauseAfterNextUpdate = false;
// 		m_isPaused = true;
// 	}

}


void App::Render() const
{
	g_engine->m_render->ClearScreen(Rgba8(0, 0, 0, 255));

	//Temp Fix------------------------------------------------------
	g_engine->m_render->SetBlendMode(BlendMode::ALPHA);
	g_engine->m_render->BindTexture(nullptr);
	g_engine->m_render->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_engine->m_render->BindShader(nullptr);
	//--------------------------------------------------------------
	m_game->Render();

	g_engine->m_render->BeginCamera(*m_gameCamera);
	if (g_engine->m_devConsole->IsOpen())
	{
		AABB2 consoleBounds(Vec2(0.f, 0.f), m_gameCamera->GetOrthographicTopRight());
		g_engine->m_devConsole->Render(consoleBounds);
	}
	g_engine->m_render->EndCamera(*m_gameCamera);
}


void App::SetIsQuitting()
{
	m_isQuitting = true;
}

bool App::SetIsQuittingEvent([[maybe_unused]] EventArgs& args)
{
	g_theApp->m_isQuitting = true;
	return true;
}


bool App::Command_SetGameTimeScale(EventArgs& args)
{
	std::string scaleStr = args.GetValue("arg1", "");

	if (scaleStr.empty())
	{
		g_engine->m_devConsole->AddLine(Rgba8(255, 80, 80, 255),
			"ERROR: Syntax: set_game_time_scale <scale>");
		return true;
	}

	float scale = (float)atof(scaleStr.c_str());
	if (!_finite(scale) || scale < 0.0f)
	{
		g_engine->m_devConsole->AddLine(Rgba8(255, 80, 80, 255),
			"ERROR: Syntax: set_game_time_scale <scale>");
		return true;
	}

	if (g_theApp->m_isPaused)
	{
		g_theApp->m_timeScaleBeforePause = scale;
	}
	else
	{
		g_theApp->m_game->GetClock()->SetTimeScale(scale);
	}

	g_engine->m_devConsole->AddLine(Rgba8(80, 255, 120, 255),
		Stringf("Game time scale set to %.3f", scale));

	return true;
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}


void App::UpdateFromKeyboard()
{
	InputSystem* input = g_engine->m_input;

	if (g_engine->m_devConsole->IsOpen())
		return;

	if (input->WasKeyJustPressed('Q'))
	{
		SetIsQuitting();
	}

	if (input->WasKeyJustPressed(KEYCODE_F8))
	{
		delete m_game;
		m_game = new Game();
		m_gameCamera = m_game->m_screenCamera;

		// Reset App-owned pause/step state
		m_isPaused = false;
		m_pauseAfterNextUpdate = false;
		m_stepSingleFrameRequested = false;
		m_timeScaleBeforePause = 1.0f;

		// Make sure the new game clock is running
		m_game->GetClock()->SetTimeScale(1.0f);
	}
}

bool App::HandleKeyDown(EventArgs& args)
{
	unsigned long long keyCodeULL = args.GetValue("keyCode", 0ULL);
	unsigned char keyCode = (unsigned char)keyCodeULL;
	if (keyCode == 'P')
	{
		m_game->GetClock()->TogglePause();
		return true;
	}

	if (keyCode == 'O')
	{
		m_stepSingleFrameRequested = true;
		m_game->GetClock()->StepSingleFrame();
		return true;
	}

	if (keyCode == 'T')
	{
		float current = m_isPaused ? m_timeScaleBeforePause : (float)m_game->GetClock()->GetTimeScale();
		float next = (fabsf(current - 1.f) < 0.0001f) ? 0.1f : 1.f;

		if (m_isPaused)
		{
			m_timeScaleBeforePause = next;
		}
		else
		{
			m_game->GetClock()->SetTimeScale(next);
		}
		return true;
	}
	return false;
}

bool App::OnKeyDownEvent(EventArgs& args)
{
	return g_theApp->HandleKeyDown(args);
}
