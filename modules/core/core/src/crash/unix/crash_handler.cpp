#include "../unix/crash_handler.hpp"

SUnixCrashHandler::SUnixCrashHandler() AI_NOEXCEPT : SCrashHandler()
{
}

SUnixCrashHandler::~SUnixCrashHandler() AI_NOEXCEPT
{
}

#ifndef UNIX_CRASH_HANDLER_IMPLEMENTED
namespace
{
SUnixCrashHandler unix_crash_handler;
}
extern "C"
{
    AI_CORE_API SCrashHandlerId ai_initialize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::unix_crash_handler;
        this_.Initialize();
        return &this_;
    }

    AI_CORE_API SCrashHandlerId ai_crash_handler_get() AI_NOEXCEPT
    {
        return &::unix_crash_handler;
    }

    AI_CORE_API void ai_finalize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::unix_crash_handler;
        this_.Finalize();
    }
}
#endif