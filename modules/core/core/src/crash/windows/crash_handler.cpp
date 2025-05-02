#include "./../../winheaders.h"

#include "AICore/crash.h"
#include "AICore/fmt.hpp"
#include "AICore/logger.hpp"

#include <Dbghelp.h>
#include <signal.h>
#include <string>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "User32.lib")

namespace
{
static const char *kDebugHelpDLLName = "dbghelp.dll";
struct WinCrashHandler : public SCrashHandler
{
    WinCrashHandler() AI_NOEXCEPT = default;
    virtual ~WinCrashHandler() AI_NOEXCEPT;

    WinCrashHandler(WinCrashHandler const &) = delete;
    WinCrashHandler &operator=(WinCrashHandler const &) = delete;

    bool Initialize() AI_NOEXCEPT override;
    bool Finalize() AI_NOEXCEPT override;
    bool SetProcessSignalHandlers() AI_NOEXCEPT override;
    bool SetThreadSignalHandlers() AI_NOEXCEPT override;
    bool UnsetProcessSignalHandlers() AI_NOEXCEPT override;
    bool UnsetThreadSignalHandlers() AI_NOEXCEPT override;
    void terminateProcess(int32_t code = 1) AI_NOEXCEPT override
    {
        TerminateProcess(GetCurrentProcess(), code);
    }

    SCrashContext ctx;
    SCrashContext *getCrashContext(CrashTerminateCode reason) AI_NOEXCEPT override
    {
        ctx.reason = reason;
        return &ctx;
    }
    int internalHandler(struct SCrashContext *context) AI_NOEXCEPT;

  private:
    HANDLE dbghelp_dll = nullptr;
    bool initialized = false;

  private:
    static DWORD WINAPI StackOverflowThreadFunction(LPVOID lpParameter) AI_NOEXCEPT;
    static LONG WINAPI SehHandler(PEXCEPTION_POINTERS pExceptionPtrs) AI_NOEXCEPT;
    // C++ terminate handler
    static void WINAPI TerminateHandler() AI_NOEXCEPT;
    // C++ unexpected handler
    static void WINAPI UnexpectedHandler() AI_NOEXCEPT;
#if _MSC_VER >= 1300
    static int WINAPI NewHandler(size_t sz) AI_NOEXCEPT;
    static void WINAPI PureCallHandler() AI_NOEXCEPT;
#endif
#if _MSC_VER >= 1400
    static void WINAPI InvalidParameterHandler(const wchar_t *expression, const wchar_t *function, const wchar_t *file,
                                               unsigned int line, uintptr_t pReserved) AI_NOEXCEPT;
#endif

    LPTOP_LEVEL_EXCEPTION_FILTER prevExceptionFilter = nullptr;
    terminate_handler prevTerm = nullptr;
    unexpected_handler prevUnexp = nullptr;
#if _MSC_VER >= 1300
    _purecall_handler prevPurec = nullptr;
    _PNH prevNewHandler = nullptr;
#endif
#if _MSC_VER >= 1400
    _invalid_parameter_handler prevInvpar = nullptr;
#endif
};

WinCrashHandler windows_crash_handler;

WinCrashHandler::~WinCrashHandler() AI_NOEXCEPT
{
}

bool WinCrashHandler::Initialize() AI_NOEXCEPT
{
    crashSetErrorMsg("Unspecified error.");

    // load dbghelp.dll
    dbghelp_dll = ::LoadLibraryA((const char *)kDebugHelpDLLName);
    if (!dbghelp_dll)
    {
        crashSetErrorMsg("Failed to load dbghelp.dll.");
        return false;
    }

    if (!SCrashHandler::Initialize())
    {
        return false;
    }

    // The following code is intended to fix the issue with 32-bit applications in 64-bit environment.
    // http://support.microsoft.com/kb/976038/en-us
    // http://code.google.com/p/crashrpt/issues/detail?id=104
    typedef BOOL(WINAPI * SETPROCESSUSERMODEEXCEPTIONPOLICY)(DWORD dwFlags);
    typedef BOOL(WINAPI * GETPROCESSUSERMODEEXCEPTIONPOLICY)(LPDWORD lpFlags);
#define PROCESS_CALLBACK_FILTER_ENABLED 0x1
    HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
    if (hKernel32 != NULL)
    {
        SETPROCESSUSERMODEEXCEPTIONPOLICY pfnSetProcessUserModeExceptionPolicy =
            (SETPROCESSUSERMODEEXCEPTIONPOLICY)GetProcAddress(hKernel32, "SetProcessUserModeExceptionPolicy");
        GETPROCESSUSERMODEEXCEPTIONPOLICY pfnGetProcessUserModeExceptionPolicy =
            (GETPROCESSUSERMODEEXCEPTIONPOLICY)GetProcAddress(hKernel32, "GetProcessUserModeExceptionPolicy");
        if (pfnSetProcessUserModeExceptionPolicy != NULL && pfnGetProcessUserModeExceptionPolicy != NULL)
        {
            DWORD dwFlags = 0;
            if (pfnGetProcessUserModeExceptionPolicy(&dwFlags))
            {
                pfnSetProcessUserModeExceptionPolicy(dwFlags & ~PROCESS_CALLBACK_FILTER_ENABLED);
            }
        }
        FreeLibrary(hKernel32);
    }

    // TODO: deadlock detection
    // we can create another process that monitors deadlocks in current process.

    crashSetErrorMsg("Sucessfully initialized crash handler.");
    initialized = true;

    ai_crash_handler_add_callback(
        this,
        +[](struct SCrashContext *context, void *usr_data) -> int {
            auto this_ = reinterpret_cast<WinCrashHandler *>(usr_data);
            return this_->internalHandler(context);
        },
        this);

    return initialized;
}

int WinCrashHandler::internalHandler(struct SCrashContext *context) AI_NOEXCEPT
{
    if (!context->exception_pointers)
        context->exception_pointers = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

    const auto type = MB_ABORTRETRYIGNORE | MB_ICONERROR;
    const auto reason = context->reason;
    std::string why = fmt::format("Crashed! Reason: {}", ai_crash_code_string(reason));

    AI_LOG_CRITICAL("{}", why.c_str());

    // save crash minidump
    {
        SYSTEMTIME localTime;
        ::GetLocalTime(&localTime);

        std::string dateTime =
            fmt::format("{}-{}-{}-{}-{}-{}-{}", localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour,
                        localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);

        std::string dumpPath = fmt::format("minidump-{}.dmp", dateTime);
        const char *pDumpPath = dumpPath.c_str();

        HANDLE lhDumpFile =
            ::CreateFileA(pDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
        loExceptionInfo.ExceptionPointers = context->exception_pointers;
        loExceptionInfo.ThreadId = ::GetCurrentThreadId();
        loExceptionInfo.ClientPointers = TRUE;

        ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), lhDumpFile, MiniDumpNormal,
                            &loExceptionInfo, NULL, NULL);

        ::CloseHandle(lhDumpFile);
    }

    // show message box
    ::MessageBoxExA(nullptr, why.c_str(), "Crashed!", type, 0);

    return 0;
}

bool WinCrashHandler::Finalize() AI_NOEXCEPT
{
    return SCrashHandler::Finalize();
}

DWORD WINAPI WinCrashHandler::StackOverflowThreadFunction(LPVOID lpParameter) AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    PEXCEPTION_POINTERS pExceptionPtrs = reinterpret_cast<PEXCEPTION_POINTERS>(lpParameter);
    const auto reason = kCrashCodeStackOverflow;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                ctx->exception_pointers = pExceptionPtrs;
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
    return 0;
}

void WINAPI WinCrashHandler::TerminateHandler() AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    const auto reason = kCrashCodeTerminate;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

void WINAPI WinCrashHandler::UnexpectedHandler() AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    const auto reason = kCrashCodeUnexpected;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

LONG WINAPI WinCrashHandler::SehHandler(PEXCEPTION_POINTERS pExceptionPtrs) AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    if (!this_.initialized)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Handle stack overflow in a separate thread.
    // Vojtech: Based on martin.bis...@gmail.com comment in
    //	http://groups.google.com/group/crashrpt/browse_thread/thread/a1dbcc56acb58b27/fbd0151dd8e26daf?lnk=gst&q=stack+overflow#fbd0151dd8e26daf
    const auto hasExceptionRecord = pExceptionPtrs && pExceptionPtrs->ExceptionRecord;
    const auto isStackOverflow =
        hasExceptionRecord && pExceptionPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW;
    if (isStackOverflow)
    {
        // Special case to handle the stack overflow exception.
        // The dump will be realized from another thread.
        // Create another thread that will do the dump.
        HANDLE thread = ::CreateThread(0, 0, &StackOverflowThreadFunction, pExceptionPtrs, 0, 0);
        ::WaitForSingleObject(thread, INFINITE);
        ::CloseHandle(thread);

        const auto reason = kCrashCodeUnhandled;
        this_.handleFunction(
            [&]() {
                this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                    auto ctx = this_.getCrashContext(reason);
                    ctx->exception_pointers = pExceptionPtrs;
                    wrapper.callback(ctx, wrapper.usr_data);
                });
            },
            reason);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

#if _MSC_VER >= 1300
void WINAPI WinCrashHandler::PureCallHandler() AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    const auto reason = kCrashCodePureVirtual;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}

int WINAPI WinCrashHandler::NewHandler(size_t sz) AI_NOEXCEPT
{
    auto &this_ = windows_crash_handler;
    const auto reason = kCrashCodeOpNewError;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
    return 0;
}
#endif

#if _MSC_VER >= 1400
void WINAPI WinCrashHandler::InvalidParameterHandler(const wchar_t *expression, const wchar_t *function,
                                                     const wchar_t *file, unsigned int line,
                                                     uintptr_t pReserved) AI_NOEXCEPT
{
    (void)pReserved;
    auto &this_ = windows_crash_handler;
    const auto reason = kCrashCodeInvalidParam;
    this_.handleFunction(
        [&]() {
            this_.visit_callbacks([&](const CallbackWrapper &wrapper) {
                auto ctx = this_.getCrashContext(reason);
                wrapper.callback(ctx, wrapper.usr_data);
            });
        },
        reason);
}
#endif

bool WinCrashHandler::SetProcessSignalHandlers() AI_NOEXCEPT
{
    prevExceptionFilter = ::SetUnhandledExceptionFilter(&SehHandler);

    _set_error_mode(_OUT_TO_STDERR);

#if _MSC_VER >= 1300
    // Catch pure virtual function calls.
    // Because there is one _purecall_handler for the whole process,
    // calling this function immediately impacts all threads. The last caller on any thread sets the handler.
    // http://msdn.microsoft.com/en-us/library/t296ys27.aspx
    prevPurec = _set_purecall_handler(PureCallHandler);

    // Catch new operator memory allocation exceptions
    _set_new_mode(1); // Force malloc() to call new handler too
    prevNewHandler = _set_new_handler(NewHandler);
#endif

#if _MSC_VER >= 1400
    // Catch invalid parameter exceptions.
    prevInvpar = _set_invalid_parameter_handler(InvalidParameterHandler);

    _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
#endif

    return SCrashHandler::SetProcessSignalHandlers();
}

bool WinCrashHandler::SetThreadSignalHandlers() AI_NOEXCEPT
{
    prevTerm = set_terminate(TerminateHandler);
    prevUnexp = set_unexpected(UnexpectedHandler);
    return SCrashHandler::SetThreadSignalHandlers();
}

bool WinCrashHandler::UnsetProcessSignalHandlers() AI_NOEXCEPT
{
    if (prevExceptionFilter)
        ::SetUnhandledExceptionFilter(prevExceptionFilter);

#if _MSC_VER >= 1300
    if (prevPurec)
        _set_purecall_handler(prevPurec);
    if (prevNewHandler)
        _set_new_handler(prevNewHandler);
#endif

#if _MSC_VER >= 1400
    if (prevInvpar)
        _set_invalid_parameter_handler(prevInvpar);
#endif

    return SCrashHandler::UnsetProcessSignalHandlers();
}

bool WinCrashHandler::UnsetThreadSignalHandlers() AI_NOEXCEPT
{
    if (prevTerm != NULL)
        set_terminate(prevTerm);

    if (prevUnexp != NULL)
        set_unexpected(prevUnexp);

    return SCrashHandler::UnsetThreadSignalHandlers();
}
} // namespace

extern "C"
{

    AI_CORE_API SCrashHandlerId ai_initialize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::windows_crash_handler;
        this_.Initialize();
        return &this_;
    }

    AI_CORE_API SCrashHandlerId ai_crash_handler_get() AI_NOEXCEPT
    {
        return &::windows_crash_handler;
    }

    AI_CORE_API void ai_finalize_crash_handler() AI_NOEXCEPT
    {
        auto &this_ = ::windows_crash_handler;
        this_.Finalize();
    }
}
