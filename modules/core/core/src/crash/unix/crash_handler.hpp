#pragma once
#include "AICore/crash.h"

struct SUnixCrashHandler : public SCrashHandler
{
    SUnixCrashHandler() AI_NOEXCEPT;
    virtual ~SUnixCrashHandler() AI_NOEXCEPT;
};