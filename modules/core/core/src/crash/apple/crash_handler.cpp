#include "../unix/crash_handler.hpp"
#include "AICore/crash.h"

namespace
{
struct MacCrashHandler : public SUnixCrashHandler
{
    MacCrashHandler() AI_NOEXCEPT : SUnixCrashHandler()
    {
    }

    bool Initialize() AI_NOEXCEPT
    {
        return SUnixCrashHandler::Initialize();
    }
};
MacCrashHandler mac_crash_handler;
} // namespace

extern "C"
{
    AI_CORE_API SCrashHandlerId AI_initialize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::mac_crash_handler;
        this_.Initialize();
        return &this_;
    }

    AI_CORE_API SCrashHandlerId AI_crash_handler_get() AI_NOEXCEPT
    {
        return &::mac_crash_handler;
    }

    AI_CORE_API void AI_finalize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::mac_crash_handler;
        this_.Finalize();
    }
}