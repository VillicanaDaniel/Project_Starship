#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"

Timer::Timer(double period, const Clock* clock)
	: m_clock(clock ? clock : &Clock::GetSystemClock())
	, m_period(period)
{
}

void Timer::Start()
{
	if (!m_clock)
	{
		m_clock = &Clock::GetSystemClock();
	}
	m_startTime = m_clock->GetTotalSeconds();
}

void Timer::Stop()
{
	m_startTime = -1.0;
}

bool Timer::IsStopped() const
{
	return m_startTime < 0.0;
}

double Timer::GetElapsedTime() const
{
	if (IsStopped() || !m_clock)
		return 0.0;

	return m_clock->GetTotalSeconds() - m_startTime;
}

double Timer::GetElapsedFraction() const
{
	if (m_period <= 0.0)
		return 1.0;

	double elapsed = GetElapsedTime();
	double frac = elapsed / m_period;
	return frac;
}

bool Timer::HasPeriodElapsed() const
{
	if (IsStopped())
		return false;

	return GetElapsedTime() >= m_period;
}

bool Timer::DecrementPeriodIfElapsed()
{
	if (!HasPeriodElapsed())
		return false;

	while (HasPeriodElapsed())
	{
		m_startTime += m_period;
	}

	return true;
}