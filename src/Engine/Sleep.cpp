#include "stdafx.h"
#include "Sleep.h"
#include "EngineTime.h"
//-----------------------------------------------------------------------------
void Sleep(Time duration)
{
	if (duration >= Time::Zero)
	{
		// Get the minimum supported timer resolution on this system
		static const UINT periodMin = []
			{
				TIMECAPS tc;
				timeGetDevCaps(&tc, sizeof(TIMECAPS));
				return tc.wPeriodMin;
			}();

		// Set the timer resolution to the minimum for the Sleep call
		timeBeginPeriod(periodMin);

		// Wait...
		::Sleep(static_cast<DWORD>(duration.AsMilliseconds()));

		// Reset the timer resolution back to the system default
		timeEndPeriod(periodMin);
	}

}
//-----------------------------------------------------------------------------