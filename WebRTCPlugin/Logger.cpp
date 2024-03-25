#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#include "Logger.h"

#include "WebRTCPlugin.h"

#if _DEBUG
#include <cstdarg>
#endif

namespace unity
{
	namespace webrtc
    {
        void JLogPrint(rtc::LoggingSeverity severity, const char* fmt, ...)
        {
#if LOGGING_ENABLED
            const char* severityStr = "";
            switch (severity) {
            case rtc::LoggingSeverity::LS_VERBOSE:
                severityStr = "VERBOSE";
                break;
            case rtc::LoggingSeverity::LS_INFO:
                severityStr = "INFO";
                break;
            case rtc::LoggingSeverity::LS_WARNING:
                severityStr = "WARNING";
                break;
            case rtc::LoggingSeverity::LS_ERROR:
                severityStr = "ERROR";
                break;
            case rtc::LoggingSeverity::LS_NONE:
                return; // Do nothing for LS_NONE severity
            }

            // Print severity and message using printf
            std::printf("[%s] ", severityStr);
            va_list args;
            va_start(args, fmt);
            std::vprintf(fmt, args);
            va_end(args);
            std::printf("\n");
#endif
        }

        void DebugError(const char* fmt, ...) {
            JLogPrint(rtc::LS_ERROR, fmt);
        }

        void checkf(bool result, const char* msg)
        {
            if (!result)
            {
                throw std::runtime_error(msg);
            }
        }
	}
}