#include "AIBase/misc/hash.h"
#include "AIBase/misc/hash.hpp"
#include "AICore/crash.h"
#include "AICore/logger.hpp"
#include "range/v3/all.hpp"
#include <cstdint>

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

void test_range()
{
    using namespace ranges;
    auto vec = views::iota(1, 5) | views::transform([](int v) { return v * 2; }) | ranges::to<std::vector>();
    AI_LOG_INFO("vec: ");
    for_each(vec, [](int i) { AI_LOG_INFO("{}", i); });
}

int main(int argc, char *argv[])
{
    AI_LOG_ERROR("hello world");

    uint8_t i = 156;
    const auto hash_i = Hash<uint8_t>()(i);
    AI_LOG_INFO("i: {},hash:{}", i, hash_i);

    std::string str = "fdsafds";
    auto hash_str = ai_hash(str.data(), str.size(), AI_DEFAULT_HASH_SEED_64);
    AI_LOG_INFO("str: {},hash:{}", str, hash_str);

    test_range();

    // int *b = nullptr;
    // *b = 9;

    return 0;
}
