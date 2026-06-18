#pragma once
#include <vector>

class Clock
{
public:
// 	Clock();
// 
// 	explicit Clock();

	Clock(Clock* parent = nullptr);
	Clock(const Clock& copy) = delete;

	~Clock();

	void Tick();
	void Reset();
	bool isPaused() const;
	void Pause();
	void Unpause();
	void TogglePause();

	void StepSingleFrame();
	void SetTimeScale(double timeScale);
	double GetTimeScale() const;

	double GetDeltaSeconds() const;
	double GetTotalSeconds() const;
	double GetFrameRate() const;
	int GetFrameCount() const;

public:
	static Clock& GetSystemClock();
	static void TickSystemClock();

protected:
	void Advance(double deltaTimeSeconds);
	void AddChild(Clock* childClock);
	void RemoveChild(Clock* childClock);

protected:
	Clock* m_parent = nullptr;
	std::vector<Clock*> m_children;

	double m_lastUpdateTimeInSeconds = 0.0;
	double m_totalSeconds = 0.0;
	double m_deltaSeconds = 0.0;
	int m_frameCount = 0;

	double m_timeScale = 1.0;
	bool m_isPaused = false;
	bool m_stepSingleFrame = false;
	double m_maxDeltaSeconds = 0.1;
};