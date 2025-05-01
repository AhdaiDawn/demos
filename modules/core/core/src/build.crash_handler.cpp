#include "AICore/crash_handler.hpp"
#include "AICore/logger.hpp"
#include <map>
#include <string>
#include <vector>

#include <client/crash_report_database.h>
#include <client/crashpad_client.h>
#include <client/crashpad_info.h>
#include <client/settings.h>

namespace AI
{
bool crashpad_handler(int, siginfo_t*, ucontext_t*) {
    AI_LOG_ERROR("crashpad_handler");
    Logger::Get()->flush_log();
    return false;
}

bool CrashHandler::StartCrashHandler(const std::string &handler_path, const std::string &db_path,
                                     const std::string &url)
{
    using namespace crashpad;
    std::map<std::string, std::string> annotations;
    std::vector<std::string> arguments;

    /*
     * THE FOLLOWING ANNOTATIONS MUST BE SET.
     *
     * Backtrace supports many file formats. Set format to minidump
     * so it knows how to process the incoming dump.
     */
    annotations["format"] = "minidump";

    /*
     * REMOVE THIS FOR ACTUAL BUILD.
     *
     * We disable crashpad rate limiting for this example.
     */
    arguments.push_back("--no-rate-limit");

    base::FilePath db(db_path);
    base::FilePath handler(handler_path);

    auto database = crashpad::CrashReportDatabase::Initialize(db);

    if (database == nullptr || database->GetSettings() == NULL)
        return false;

    /* Enable automated uploads. */
    database->GetSettings()->SetUploadsEnabled(true);

    bool ret = CrashpadClient{}.StartHandler(handler, db, db, url, "", annotations, arguments, true, false);
    if (!ret)
        return false;
    crashpad::CrashpadClient::SetFirstChanceExceptionHandler(&crashpad_handler);
    return true;
}

} // namespace AI
