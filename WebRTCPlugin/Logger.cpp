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
        void LogPrint(rtc::LoggingSeverity severity, const char* fmt, ...)
        {
#if _DEBUG
            va_list vl;
            va_start(vl, fmt);
            char buf[2048];
#if _WIN32
            vsprintf_s(buf, fmt, vl);
#else
            vsprintf(buf, fmt, vl);
#endif
            switch (severity)
            {
            case rtc::LoggingSeverity::LS_ERROR:
                printf("LS_ERROR: %s", severity, buf);
                break;
            default:
                break;
            }
            printf("", severity, buf);
            va_end(vl);
#endif
        }

        void DebugError(const char* fmt, ...) {
            LogPrint(rtc::LS_ERROR, fmt);
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