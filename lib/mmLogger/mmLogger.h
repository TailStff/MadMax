#pragma once

#include <Arduino.h>
#include <cstdint>

namespace MadMax
{

enum class LogLevel : uint8_t
{
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
    None
};

class Logger
{
public:

    using OutputCallback =
        void (*)(LogLevel level,
                 const char* category,
                 const char* message);

    static void Begin(OutputCallback callback);

    static void SetLevel(LogLevel level);
    static LogLevel GetLevel();

    static void Log(LogLevel level,
                    const char* category,
                    const char* format,
                    ...);

    static const char* LevelToString(LogLevel level);

private:

    static LogLevel _level;
    static OutputCallback _callback;

    static void Write(LogLevel level,
                      const char* category,
                      const char* message);
};

} // namespace MadMax


#define MM_LOG_TRACE(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Trace, category, __VA_ARGS__)

#define MM_LOG_DEBUG(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Debug, category, __VA_ARGS__)

#define MM_LOG_INFO(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Info, category, __VA_ARGS__)

#define MM_LOG_WARN(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Warn, category, __VA_ARGS__)

#define MM_LOG_ERROR(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Error, category, __VA_ARGS__)

#define MM_LOG_FATAL(category, ...) \
    MadMax::Logger::Log(MadMax::LogLevel::Fatal, category, __VA_ARGS__)