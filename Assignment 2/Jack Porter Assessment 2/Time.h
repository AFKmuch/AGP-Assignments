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

	int m_framesPerSecond;
	int m_frameCount;
	float m_secondCount;

	bool m_stopped;
public:
	Time();
	~Time();

	float TotalTime()const;
	float DeltaTime()const;
	int	  GetFramesPerSecond()const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	static Time* Instance();
};

