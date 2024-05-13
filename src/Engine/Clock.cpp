#include "stdafx.h"
#include "Clock.h"
#include "EngineTime.h"
//-----------------------------------------------------------------------------
Time Clock::GetElapsedTime() const
{
	if (IsRunning())
		return std::chrono::duration_cast<std::chrono::microseconds>(ClockImpl::now() - m_refPoint);
	return std::chrono::duration_cast<std::chrono::microseconds>(m_stopPoint - m_refPoint);
}
//-----------------------------------------------------------------------------
bool Clock::IsRunning() const
{
	return m_stopPoint == ClockImpl::time_point();
}
//-----------------------------------------------------------------------------
void Clock::Start()
{
	if (!IsRunning())
	{
		m_refPoint += ClockImpl::now() - m_stopPoint;
		m_stopPoint = {};
	}
}
//-----------------------------------------------------------------------------
void Clock::Stop()
{
	if (IsRunning())
		m_stopPoint = ClockImpl::now();
}
//-----------------------------------------------------------------------------
Time Clock::Restart()
{
	const Time elapsed = GetElapsedTime();
	m_refPoint = ClockImpl::now();
	m_stopPoint = {};
	return elapsed;
}
//-----------------------------------------------------------------------------
Time Clock::Reset()
{
	const Time elapsed = GetElapsedTime();
	m_refPoint = ClockImpl::now();
	m_stopPoint = m_refPoint;
	return elapsed;
}
//-----------------------------------------------------------------------------