package("crashpad")
    set_homepage("https://chromium.googlesource.com/crashpad/crashpad/+/refs/heads/main/README.md")
    set_description("Crashpad is a crash-reporting system.")
    set_license("Apache-2.0")

    add_urls("https://github.com/getsentry/crashpad.git")
    add_versions("2025.03.25-ai", "2d97a484bb628b05a46203e4a05c6c827796e5cd")

    add_includedirs("include/crashpad", "include/crashpad/mini_chromium")
    add_links("crashpad_client", "crashpad_util", "mini_chromium")
    if add_bindirs then
        add_bindirs("bin")
    end

    add_deps("cmake")
    add_deps("libcurl")
    add_deps("zlib")

    if is_plat("macosx") then
        add_frameworks("CoreFoundation")
        add_syslinks("bsm")
    end

    on_install("linux", "windows|x64", "windows|x86", "macosx", function(package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs, {packagedeps = "libcurl"})
        if not package:get("bindirs") then
            package:addenv("PATH", "bin")
        end
    end)

    on_test(function(package)
        if not package:is_cross() then
            os.vrun("crashpad_handler --help")
        end

        assert(package:check_cxxsnippets({test = [[
            #include "client/crashpad_client.h"
            using namespace crashpad;
            void test() {
                CrashpadClient *client = new CrashpadClient();
            }
        ]]}, {configs = {languages = "cxx17"}}))
    end)
