add_requires("quill >=9.0.2-ai")

shared_module("AICore", "AI_CORE")
    add_packages("quill", {public = true})
    if not has_config("shipping_one_archive") then
        add_defines("QUILL_DLL_EXPORT", {public = false})
        add_defines("QUILL_DLL_IMPORT", {interface = true})
    end

    add_deps("xxhash", {public = true})
    add_deps("stduuid", {public = true})
    add_deps("nlohmann_json", {public = true})
    add_deps("ctre", {public = true})
    add_deps("range-v3", {public = true})

    add_includedirs("include", {public = true})
    add_files("src/build.*.c", "src/build.*.cpp")

    -- platform
    if (is_os("windows")) then
        add_syslinks("advapi32", "user32", "shell32", "Ole32", "Shlwapi", {public = true})
    else
        add_syslinks("pthread")
    end
