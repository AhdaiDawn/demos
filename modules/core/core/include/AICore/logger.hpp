#pragma once
#include "AIBase/config.h" // IWYU pragma: export
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include <memory>

namespace AI
{
constexpr const char *DEFAULT_LOG_NAME = "AI";

class AI_CORE_API Logger
{
  public:
    static auto Get() -> quill::Logger *;

    static auto CreateOrGet(const char *name = DEFAULT_LOG_NAME) -> quill::Logger *;

  protected:
    Logger() noexcept;
    ~Logger();

  public:
    DISABLE_COPY_AND_MOVE(Logger)

  private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

#define AI_LOG_TRACE_L3(fmt, ...) QUILL_LOG_TRACE_L3(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_TRACE_L2(fmt, ...) QUILL_LOG_TRACE_L2(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_TRACE_L1(fmt, ...) QUILL_LOG_TRACE_L1(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_DEBUG(fmt, ...) QUILL_LOG_DEBUG(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_INFO(fmt, ...) QUILL_LOG_INFO(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_WARNING(fmt, ...) QUILL_LOG_WARNING(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_ERROR(fmt, ...) QUILL_LOG_ERROR(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_CRITICAL(fmt, ...) QUILL_LOG_CRITICAL(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_BACKTRACE(fmt, ...) QUILL_LOG_BACKTRACE(AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_DYNAMIC(log_level, fmt, ...) QUILL_LOG_DYNAMIC(AI::Logger::Get(), log_level, fmt, ##__VA_ARGS__)

#define AI_LOG_TRACE_L3_LIMIT(min_interval, fmt, ...)                                                                  \
    QUILL_LOG_TRACE_L3_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_TRACE_L2_LIMIT(min_interval, fmt, ...)                                                                  \
    QUILL_LOG_TRACE_L2_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_TRACE_L1_LIMIT(min_interval, fmt, ...)                                                                  \
    QUILL_LOG_TRACE_L1_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_DEBUG_LIMIT(min_interval, fmt, ...)                                                                     \
    QUILL_LOG_DEBUG_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_INFO_LIMIT(min_interval, fmt, ...)                                                                      \
    QUILL_LOG_INFO_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_WARNING_LIMIT(min_interval, fmt, ...)                                                                   \
    QUILL_LOG_WARNING_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_ERROR_LIMIT(min_interval, fmt, ...)                                                                     \
    QUILL_LOG_ERROR_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)
#define AI_LOG_CRITICAL_LIMIT(min_interval, fmt, ...)                                                                  \
    QUILL_LOG_CRITICAL_LIMIT(min_interval, AI::Logger::Get(), fmt, ##__VA_ARGS__)

} // namespace AI
