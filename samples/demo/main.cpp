#include "AIBase/misc/hash.h"
#include "AICore/crash_handler.hpp"
#include "AICore/logger.hpp"

using namespace AI;

static struct ProcInitializer
{
    ProcInitializer()
    {
        bool ret = CrashHandler::StartCrashHandler("crashpad_handler", "./crashpad_db", "");
        if (!ret)
            AI_LOG_ERROR("Failed to start crash handler");
        Logger::Get()->flush_log();
        Logger::Get()->set_log_level(quill::LogLevel::Info);
    }
    ~ProcInitializer()
    {
        Logger::Get()->flush_log();
    }
} init;

int main(int argc, char *argv[])
{
    AI_LOG_ERROR("hello world");

    std::string str = "fdsafds";
    auto hash = ai_hash(str.data(), str.size(), AI_DEFAULT_HASH_SEED_64);
    AI_LOG_INFO("str: {},hash:{}", str, hash);

    int *b = nullptr;
    *b = 9;

    return 0;
}