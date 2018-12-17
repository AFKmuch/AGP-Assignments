#pragma once
#include <Windows.h>
class Time
{
private:
	static Time* s_instance;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_previousTime;
	__int64 m_currentTime;

	double m_secondsPerCount;
	double m_deltaTime;

	bool m_stopped;
public:
	Time();
	~Time();

	float TotalTime()const;
	float DeltaTime()const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	static Time* Instance();
};

