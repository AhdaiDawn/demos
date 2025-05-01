#pragma once
#include "AIBase/config.h" // IWYU pragma: export
#include <string>

namespace AI
{
class AI_CORE_API CrashHandler
{
  public:
    static bool StartCrashHandler(const std::string &handler_path, const std::string &db_path,
                                  const std::string &url = "");
};
} // namespace AI
