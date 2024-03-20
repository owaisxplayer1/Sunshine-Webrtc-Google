#pragma once
#include "rtc_base/logging.h"

namespace unity
{
	namespace webrtc
	{
		void LogPrint(rtc::LoggingSeverity severity, const char* fmt, ...);
		void DebugError(const char* fmt, ...);
	}
}