#pragma once

class Clock;

class Timer
{
public:
	explicit Timer(double period, const Clock* clock = nullptr);
	void Start();
	void Stop();
	double GetElapsedTime() const;
	double GetElapsedFraction() const;
	bool IsStopped() const;
	bool HasPeriodElapsed() const;

	bool DecrementPeriodIfElapsed();
	const Clock* m_clock = nullptr;
	double m_startTime = -1.0f;
	double m_period = 0.0f;
};