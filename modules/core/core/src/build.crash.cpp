#include "AIBase/config.h"

#include "crash/standard/crash_handler.cpp"
#if AI_PLAT_UNIX
    #if AI_PLAT_MACOSX
        #define UNIX_CRASH_HANDLER_IMPLEMENTED
        #include "crash/apple/crash_handler.cpp"
    #elif AI_PLAT_LINUX
        #define UNIX_CRASH_HANDLER_IMPLEMENTED
        #include "crash/linux/crash_handler.cpp"
    #endif
    #include "crash/unix/crash_handler.cpp"
#elif AI_PLAT_WINDOWS
    #include "crash/windows/crash_handler.cpp"
#endif