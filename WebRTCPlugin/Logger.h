#pragma once
#include "rtc_base/logging.h"

namespace unity
{
	namespace webrtc
	{
		// Define a flag to enable or disable logging
#define LOGGING_ENABLED 1

// Define macros for different logging levels
#if LOGGING_ENABLED
#define LOG_DEBUG(...)   std::cout << "[DEBUG] " << __VA_ARGS__ << std::endl;
#define LOG_INFO(...)    std::cout << "[INFO] " << __VA_ARGS__ << std::endl;
#define LOG_WARNING(...) std::cerr << "[WARNING] " << __VA_ARGS__ << std::endl;
#define LOG_ERROR(...)   std::cerr << "[ERROR] " << __VA_ARGS__ << std::endl;
#else
	// If logging is disabled, define empty macros
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#endif

		void LogPrint(rtc::LoggingSeverity severity, const char* fmt, ...);
		void DebugError(const char* fmt, ...);
	}
}