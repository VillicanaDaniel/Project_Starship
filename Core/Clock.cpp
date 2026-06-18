#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include <algorithm>

//------------------------------------------------------------------------------------------------
// System Clock singleton
//------------------------------------------------------------------------------------------------
static Clock* s_systemClock = nullptr;

//------------------------------------------------------------------------------------------------
Clock::Clock(Clock* parent)
	: m_parent(parent)
{
	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();

	if (m_parent != nullptr)
	{
		m_parent->AddChild(this);
	}
}

Clock::~Clock()
{
	// Detach from parent
	if (m_parent != nullptr)
	{
		m_parent->RemoveChild(this);
		m_parent = nullptr;
	}

	// Orphan children
	for (Clock* child : m_children)
	{
		if (child)
		{
			child->m_parent = nullptr;
		}
	}
	m_children.clear();
}

void Clock::Reset()
{
	m_totalSeconds = 0.0;
	m_deltaSeconds = 0.0;
	m_frameCount = 0;
	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();
	m_stepSingleFrame = false;
}

bool Clock::isPaused() const
{
	return m_isPaused;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Unpause()
{
	m_isPaused = false;
}

void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

void Clock::StepSingleFrame()
{
	m_isPaused = true;
	m_stepSingleFrame = true;
}

void Clock::SetTimeScale(double timeScale)
{
	m_timeScale = timeScale;
}

double Clock::GetTimeScale() const
{
	return m_timeScale;
}

double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

double Clock::GetFrameRate() const
{
	return (m_deltaSeconds > 0.0) ? (1.0 / m_deltaSeconds) : 0.0;
}

int Clock::GetFrameCount() const
{
	return m_frameCount;
}

Clock& Clock::GetSystemClock()
{
	if (s_systemClock == nullptr)
	{
		s_systemClock = new Clock(nullptr);
	}
	return *s_systemClock;
}

void Clock::TickSystemClock()
{
	Clock& sys = GetSystemClock();
	sys.Tick();
}

void Clock::Tick()
{
	double now = GetCurrentTimeSeconds();
	double rawDelta = now - m_lastUpdateTimeInSeconds;
	m_lastUpdateTimeInSeconds = now;

	if (rawDelta < 0.0)
		rawDelta = 0.0;
	if (rawDelta > m_maxDeltaSeconds)
		rawDelta = m_maxDeltaSeconds;

	if (m_timeScale == 0.0)
	{
		if (m_stepSingleFrame)
		{
			m_stepSingleFrame = false;
			m_deltaSeconds = 1.0 / 60.0;
			m_totalSeconds += m_deltaSeconds;
		}
		else
		{
			m_deltaSeconds = 0.0;
		}
		return;
	}

	Advance(rawDelta);
}

void Clock::Advance(double deltaTimeSeconds)
{
	double scaledDelta = 0.0;

	if (!m_isPaused)
	{
		scaledDelta = deltaTimeSeconds * m_timeScale;
	}
	else if (m_stepSingleFrame)
	{
		scaledDelta = deltaTimeSeconds * m_timeScale;

		m_stepSingleFrame = false;
	}

	m_deltaSeconds = scaledDelta;

	if (scaledDelta > 0.0)
	{
		m_totalSeconds += scaledDelta;
		m_frameCount++;
	}

	for (Clock* child : m_children)
	{
		if (child != nullptr)
		{
			child->Advance(m_deltaSeconds);
		}
	}
}

void Clock::AddChild(Clock* childClock)
{
	if (childClock == nullptr)
		return;

	// Avoid duplicates
	auto it = std::find(m_children.begin(), m_children.end(), childClock);
	if (it != m_children.end())
		return;

	m_children.push_back(childClock);
	childClock->m_parent = this;
}

void Clock::RemoveChild(Clock* childClock)
{
	if (childClock == nullptr)
		return;

	auto it = std::find(m_children.begin(), m_children.end(), childClock);
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
}