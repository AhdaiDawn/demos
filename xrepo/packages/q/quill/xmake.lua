package("quill")
    set_kind("library", { headeronly = true })
    set_homepage("https://github.com/odygrd/quill")
    set_description("Asynchronous Low Latency C++ Logging Library")
    set_license("MIT")

    add_versions("9.0.2-ai", "86AE2F9BA67EBE0B8C3E01A053340424420E18DA1BC0CC040D27F99B13E5CE92")

    if is_plat("linux") then
        add_syslinks("pthread")
    end

    on_install(function (package)
        os.mkdir(package:installdir())
        os.cp(path.join(package:scriptdir(), "port", "quill"), ".")
        os.cp(path.join(package:scriptdir(), "port", "xmake.lua"), "xmake.lua")

        local configs = {}
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        local code = [[
            #include "quill/Backend.h"
            #include "quill/Frontend.h"
            #include "quill/LogMacros.h"
            #include "quill/Logger.h"
            #include "quill/sinks/ConsoleSink.h"

            void test()
            {
                quill::Backend::start();
                auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
                quill::Logger* logger = quill::Frontend::create_or_get_logger("root", std::move(console_sink));
                LOG_INFO(logger, "This is a log info example {}", 123);
            }
        ]]
        assert(package:check_cxxsnippets({test = code}, {configs = {languages = "c++17"}}))
    end)
