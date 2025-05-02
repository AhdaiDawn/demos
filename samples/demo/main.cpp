#include "AIBase/misc/hash.h"
#include "AIBase/misc/hash.hpp"
#include "AICore/crash.h"
#include "AICore/logger.hpp"
#include <cstdint>
#include <quill/core/LogLevel.h>

using namespace AI;

static struct ProcInitializer
{
    ProcInitializer()
    {
        Logger::Get()->set_log_level(quill::LogLevel::Info);
        ::ai_initialize_crash_handler();
    }
    ~ProcInitializer()
    {
        Logger::Get()->flush_log();
        ::ai_finalize_crash_handler();
    }
} init;

int main(int argc, char *argv[])
{
    AI_LOG_ERROR("hello world");

    uint8_t i = 156;
    const auto hash_i = Hash<uint8_t>()(i);
    AI_LOG_INFO("i: {},hash:{}", i, hash_i);

    std::string str = "fdsafds";
    auto hash_str = ai_hash(str.data(), str.size(), AI_DEFAULT_HASH_SEED_64);
    AI_LOG_INFO("str: {},hash:{}", str, hash_str);

    int *b = nullptr;
    *b = 9;

    return 0;
}
