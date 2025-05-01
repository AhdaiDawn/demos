#include "AICore/logger.hpp"
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/RotatingFileSink.h"
#include <memory>
namespace AI
{
class Logger::Impl
{
  public:
    Impl()
    {
        mLogger = Logger::CreateOrGet();
    }

    auto GetLogger() -> quill::Logger *
    {
        return mLogger;
    }

  private:
    quill::Logger *mLogger = nullptr;
};

auto Logger::Get() -> quill::Logger *
{
    static Logger s_instance;
    return s_instance.mImpl->GetLogger();
}

auto Logger::CreateOrGet(const char *name) -> quill::Logger *
{
    quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);
    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("console_sink", []() {
        quill::ConsoleSinkConfig csc;
        csc.set_override_pattern_formatter_options(quill::PatternFormatterOptions{
            "%(time) [%(process_id)] [%(thread_id)] [%(log_level)] [%(logger)] %(message)"
            "\n    \x1b[90mIn %(caller_function) At %(source_location)\x1b[0m",
            "%Y-%m-%d %H:%M:%S.%Qms"});
        return csc;
    }());

    quill::RotatingFileSinkConfig cfg;
    cfg.set_open_mode('a');
    cfg.set_max_backup_files(3);
    cfg.set_rotation_max_file_size(5 * 1024 * 1024);
    std::string filename = fmtquill::format("{}.log", name);
    auto rotating_file_sink = quill::Frontend::create_or_get_sink<quill::RotatingFileSink>(filename, cfg);

    auto logger = quill::Frontend::create_or_get_logger(
        name, {std::move(console_sink), std::move(rotating_file_sink)},
        quill::PatternFormatterOptions{"%(time) [%(process_id)] [%(thread_id)] [%(log_level)] [%(logger)] %(message)"
                                       "\n    In %(caller_function) At %(source_location)",
                                       "%Y-%m-%d %H:%M:%S.%Qms"});
    return logger;
}

Logger::Logger() noexcept : mImpl(std::make_unique<Impl>())
{
}

Logger::~Logger() = default;
} // namespace AI
