#pragma once
#include "AIBase/config.h"
#ifdef __cplusplus
    #include <mutex>
    #include <vector>

extern "C"
{
#endif

    enum
    {
        // POSIX Signals
        kCrashCodeAbort = 1,          // abort()
        kCrashCodeInterrupt = 2,      // Ctrl-C
        kCrashCodeKill = 3,           // kill
        kCrashCodeDividedByZero = 4,  // divide by zero
        kCrashCodeIllInstruction = 5, // illegal instruction
        kCrashCodeSegFault = 6,       // segmentation fault
                                      // System Exceptions
        kCrashCodeStackOverflow = 7,  // Windows
        kCrashCodeTerminate = 8,      //  (not catch) Windows/C++
        kCrashCodeUnexpected = 9,     // (throw mismatch) Windows/C++
        kCrashCodeUnhandled = 10,     // (unhandled exception) Windows
        kCrashCodePureVirtual = 12,   // (pure-virtual call) Windows
        kCrashCodeOpNewError = 13,    // (new fatal) Windows
        kCrashCodeInvalidParam = 14,  // (invalid parameter) Windows
        kCrashCodeCount
    };
    typedef int32_t CrashTerminateCode;
    AI_CORE_API const char *ai_crash_code_string(CrashTerminateCode code) AI_NOEXCEPT;

    typedef struct SCrashHandler SCrashHandler;
    typedef struct SCrashContext
    {
        CrashTerminateCode reason;
        void *usr_data;
        bool continue_execution;
#ifdef _WIN32
        struct _EXCEPTION_POINTERS *exception_pointers;
#endif
    } SCrashContext;

    typedef int (*SProcCrashCallback)(struct SCrashContext *context, void *usr_data);

    typedef struct SCrashHandler *SCrashHandlerId;

    AI_CORE_API SCrashHandlerId ai_initialize_crash_handler() AI_NOEXCEPT;
    AI_CORE_API SCrashHandlerId ai_crash_handler_get() AI_NOEXCEPT;
    AI_CORE_API void ai_crash_handler_add_callback(SCrashHandlerId handler, SProcCrashCallback callback,
                                                   void *usr_data) AI_NOEXCEPT;
    AI_CORE_API void ai_finalize_crash_handler() AI_NOEXCEPT;

#ifdef __cplusplus
}

typedef struct SCrashHandler
{
    virtual ~SCrashHandler() AI_NOEXCEPT = default;
    virtual bool Initialize() AI_NOEXCEPT;
    virtual bool Finalize() AI_NOEXCEPT;

    virtual bool SetProcessSignalHandlers() AI_NOEXCEPT;
    virtual bool UnsetProcessSignalHandlers() AI_NOEXCEPT;
    virtual bool SetThreadSignalHandlers() AI_NOEXCEPT;
    virtual bool UnsetThreadSignalHandlers() AI_NOEXCEPT;

    struct CallbackWrapper
    {
        SProcCrashCallback callback;
        void *usr_data;
    };
    template <typename F> void visit_callbacks(F &&f) AI_NOEXCEPT
    {
        std::lock_guard<std::recursive_mutex> _(callbacks_lock);
        for (auto &cb : callbacks)
        {
            f(cb);
        }
    }
    void add_callback(CallbackWrapper callback) AI_NOEXCEPT;

  protected:
    virtual void terminateProcess(int32_t code = 1) AI_NOEXCEPT;

    virtual void beforeHandlingSignal(CrashTerminateCode code) AI_NOEXCEPT;
    template <typename F> void handleFunction(F &&f, CrashTerminateCode code);
    virtual SCrashContext *getCrashContext(CrashTerminateCode reason) AI_NOEXCEPT
    {
        return nullptr;
    }

    int crashSetErrorMsg(const char *pszErrorMsg) AI_NOEXCEPT;

    std::recursive_mutex crash_lock;
    const bool kContinue = false;

    static void SigabrtHandler(int);
    static void SigfpeHandler(int, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);

    void(__cdecl *prevSigABRT)(int); // Previous SIGABRT handler.
    void(__cdecl *prevSigINT)(int);  // Previous SIGINT handler.
    void(__cdecl *prevSigTERM)(int); // Previous SIGTERM handler.

    void(__cdecl *prevSigFPE)(int);  // Previous FPE handler
    void(__cdecl *prevSigILL)(int);  // Previous SIGILL handler
    void(__cdecl *prevSigSEGV)(int); // Previous illegal storage access handler

    std::vector<CallbackWrapper> callbacks;
    std::recursive_mutex callbacks_lock;
} SCrashHandler;

template <typename F> void SCrashHandler::handleFunction(F &&f, CrashTerminateCode code)
{
    beforeHandlingSignal(code);

    auto &this_ = *ai_crash_handler_get();
    // Acquire lock to avoid other threads (if exist) to crash while we	are inside.
    {
        std::lock_guard<std::recursive_mutex> _(crash_lock);
        f();
        // Terminate process
        if (!kContinue)
        {
            this_.terminateProcess(code);
        }
    }
}
#endif
