#include "Time.h"



Time::Time() : m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0), m_pausedTime(0), m_previousTime(0), m_currentTime(0), m_stopped(0)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_secondsPerCount = 1.0 / (double)countsPerSecond;
}


Time::~Time()
{
}

float Time::TotalTime() const
{
	if (m_stopped)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
	else
	{
		return (float)(((m_currentTime - m_pausedTime) - m_baseTime) *m_secondsPerCount);
	}
	return 0.0f;
}

float Time::DeltaTime() const
{
	return (float)m_deltaTime;
}

void Time::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_baseTime = currentTime;
	m_previousTime = currentTime;
	m_stopTime = 0;
	m_stopped = false;
}

void Time::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	//Accumulate the time elapsed between stop and start pairs;

	//If we are resuming the timer from a stopped state...
	if (m_stopped)
	{
		//then accumulate the paused time.
		m_pausedTime += (startTime - m_stopTime);

		//since we are starting the timer back up, the urrent time is not valid, as it occurred while paused. so reset it to the current time;
		m_previousTime = startTime;

		//no longer stopped;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void Time::Stop()
{
	//If we are already stoppped, then don't do anything
	if (!m_stopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		//Otherwise, save the time we stopped at, and set the boolean flag indicating the timer is stopped
		m_stopTime = currentTime;
		m_stopped = true;
	}
}

void Time::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	//Get the time this frame
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_currentTime = currentTime;

	//Time difference between this frame and the previous.
	m_deltaTime = (m_currentTime - m_previousTime)*m_secondsPerCount;

	//Prepare for next frame
	m_previousTime = m_currentTime;

	//Force nonnegative. The DXSDK's CDXUTTimer mentions that if the processor goes into a power save mode or we get shuffled to another processor, then m_deltaTime can be negative
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0;
	}
}

Time * Time::Instance()
{
	if (!s_instance)
	{
		s_instance = new Time();
		s_instance->Reset();
	}
	return s_instance;
}

