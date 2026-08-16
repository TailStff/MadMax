#include "mmLogger.h"

#include <cstdarg>
#include <cstdio>

namespace MadMax
{
    LogLevel Logger::_level = LogLevel::Info;
    Logger::OutputCallback Logger::_callback = nullptr;

    void Logger::Begin(OutputCallback callback)
    {
        _callback = callback;
    }

    void Logger::SetLevel(LogLevel level)
    {
        _level = level;
    }

    LogLevel Logger::GetLevel()
    {
        return _level;
    }

    const char *Logger::LevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace:
            return "TRACE";
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO ";
        case LogLevel::Warn:
            return "WARN ";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Fatal:
            return "FATAL";
        default:
            return "NONE ";
        }
    }

    void Logger::Log(LogLevel level,
                     const char *category,
                     const char *format,
                     ...)
    {
        if (level < _level)
            return;

        char message[256];

        va_list args;
        va_start(args, format);

        vsnprintf(message,
                  sizeof(message),
                  format,
                  args);

        va_end(args);

        Write(level, category, message);
    }

    void Logger::Write(LogLevel level,
                       const char *category,
                       const char *message)
    {
        if (_callback != nullptr)
        {
            _callback(level, category, message);
        }
    }

} // namespace MadMax