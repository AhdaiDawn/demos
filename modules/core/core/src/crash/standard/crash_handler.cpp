#include "AICore/crash.h"
#include "AICore/logger.hpp"
#include <quill/LogMacros.h>
#include <signal.h>
#include <unordered_map>

std::unordered_map<uint64_t, std::string> error_map;

void SCrashHandler::terminateProcess(int32_t code) AI_NOEXCEPT
{
    ::exit(code);
}

// Sets the last error message (for the caller thread).
int SCrashHandler::crashSetErrorMsg(const char *pszErrorMsg) AI_NOEXCEPT
{
    const auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    error_map[tid] = pszErrorMsg;
    return 0;
}

bool SCrashHandler::Initialize() AI_NOEXCEPT
{
    prevSigABRT = NULL;
    prevSigINT = NULL;
    prevSigTERM = NULL;
    prevSigFPE = NULL;
    prevSigILL = NULL;
    prevSigSEGV = NULL;

    // set process exception handlers
    if (bool phdls = SetProcessSignalHandlers(); !phdls)
    {
        crashSetErrorMsg("Failed to set process exception handlers.");
        return false;
    }
    // set thread exception handlers
    if (bool thdls = SetThreadSignalHandlers(); !thdls)
    {
        crashSetErrorMsg("Failed to set thread exception handlers.");
        return false;
    }
    return true;
}

bool SCrashHandler::Finalize() AI_NOEXCEPT
{
    UnsetProcessSignalHandlers();
    UnsetThreadSignalHandlers();
    return true;
}

void SCrashHandler::SigabrtHandler(int code)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeAbort;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void SCrashHandler::SigintHandler(int code)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeInterrupt;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void SCrashHandler::SigtermHandler(int code)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeKill;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void SCrashHandler::SigfpeHandler(int code, int subcode)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeDividedByZero;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void SCrashHandler::SigillHandler(int code)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeIllInstruction;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void SCrashHandler::SigsegvHandler(int code)
{
    auto &this_ = *ai_crash_handler_get();
    const auto reason = kCrashCodeSegFault;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

bool SCrashHandler::SetProcessSignalHandlers() AI_NOEXCEPT
{
    prevSigABRT = signal(SIGABRT, SigabrtHandler); // abort
    prevSigINT = signal(SIGINT, SigintHandler);    // ctrl + c
    prevSigTERM = signal(SIGTERM, SigtermHandler); // kill
    return true;
}

bool SCrashHandler::UnsetProcessSignalHandlers() AI_NOEXCEPT
{
    if (prevSigABRT != NULL)
        signal(SIGABRT, prevSigABRT);
    if (prevSigINT != NULL)
        signal(SIGINT, prevSigINT);
    if (prevSigTERM != NULL)
        signal(SIGTERM, prevSigTERM);
    return true;
}

bool SCrashHandler::SetThreadSignalHandlers() AI_NOEXCEPT
{
    typedef void (*sigh)(int);
    prevSigFPE = signal(SIGFPE, (sigh)SigfpeHandler); // divide by zero
    prevSigILL = signal(SIGILL, SigillHandler);       // illegal instruction
    prevSigSEGV = signal(SIGSEGV, SigsegvHandler);    // segmentation fault
    return true;
}

bool SCrashHandler::UnsetThreadSignalHandlers() AI_NOEXCEPT
{
    if (prevSigFPE != NULL)
        signal(SIGFPE, prevSigFPE);
    if (prevSigILL != NULL)
        signal(SIGILL, prevSigILL);
    if (prevSigSEGV != NULL)
        signal(SIGSEGV, prevSigSEGV);
    return true;
}

void SCrashHandler::beforeHandlingSignal(CrashTerminateCode code) AI_NOEXCEPT
{
    AI_LOG_CRITICAL("SCrashHandler::beforeHandlingSignal");
    AI::Logger::Get()->flush_log();
}

void SCrashHandler::add_callback(SCrashHandler::CallbackWrapper callback) AI_NOEXCEPT
{
    std::lock_guard<std::recursive_mutex> _(callbacks_lock);
    callbacks.push_back(callback);
}

extern "C"
{

    AI_CORE_API const char *ai_crash_code_string(CrashTerminateCode code) AI_NOEXCEPT
    {
#define AI_CCODE_TRANS(code)                                                                                           \
    case kCrashCode##code:                                                                                             \
        return (const char *)#code;
        switch (code)
        {
            AI_CCODE_TRANS(Abort);
            AI_CCODE_TRANS(Interrupt);
            AI_CCODE_TRANS(Kill);
            AI_CCODE_TRANS(DividedByZero);
            AI_CCODE_TRANS(IllInstruction);
            AI_CCODE_TRANS(SegFault);
            AI_CCODE_TRANS(StackOverflow);
            AI_CCODE_TRANS(Terminate);
            AI_CCODE_TRANS(Unexpected);
            AI_CCODE_TRANS(Unhandled);
            AI_CCODE_TRANS(PureVirtual);
            AI_CCODE_TRANS(OpNewError);
            AI_CCODE_TRANS(InvalidParam);
        }
        return "";
#undef AI_CCODE_TRANS
    }

    AI_CORE_API void ai_crash_handler_add_callback(SCrashHandlerId handler, SProcCrashCallback callback,
                                                   void *usr_data) AI_NOEXCEPT
    {
        return handler->add_callback({callback, usr_data});
    }
}