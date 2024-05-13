#pragma once

/// Clock clock;
/// Time time1 = clock.GetElapsedTime();
/// Time time2 = clock.Restart();
/// Time time3 = clock.Reset();

using ClockImpl = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

static_assert(ClockImpl::is_steady, "Provided implementation is not a monotonic clock");
static_assert(std::ratio_less_equal_v<ClockImpl::period, std::micro>, "Clock resolution is too low. Expecting at least a microsecond precision");

class Time;

class Clock
{
public:
	Time GetElapsedTime() const;

	bool IsRunning() const;

	void Start();
	void Stop();

	Time Restart();
	Time Reset();
private:
	ClockImpl::time_point m_refPoint{ ClockImpl::now() }; // Time of last reset
	ClockImpl::time_point m_stopPoint;                    // Time of last stop
};