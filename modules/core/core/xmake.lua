add_requires("quill >=9.0.2-ai")
add_requires("crashpad >=2025.03.25-ai", {configs = {shared = true}})

shared_module("AICore", "AI_CORE")
    -- add source files
    add_packages("quill", {public = true})
    if not has_config("shipping_one_archive") then
        add_defines("QUILL_DLL_EXPORT", {public = false})
        add_defines("QUILL_DLL_IMPORT", {interface = true})
    end
    add_packages("crashpad", {public = true})
    before_build(function (target)
        import("lib.detect.find_program")
        local file_name = is_plat("windows") and "crashpad_handler.exe" or "crashpad_handler"
        local crashpad_handler_path = path.join(target:targetdir(), file_name)
        if not os.exists(crashpad_handler_path) then
            local p = find_program("crashpad_handler", {paths = {"$(env PATH)"}})
            p = path.directory(p).."/*"
            os.cp(p, target:targetdir())
        end
    end)
    add_deps("mimalloc", {public = false})

    add_includedirs("include", {public = true})
    add_files("src/build.*.c", "src/build.*.cpp")

    -- platform
    if (is_os("windows")) then
        add_syslinks("advapi32", "user32", "shell32", "Ole32", "Shlwapi", {public = true})
    else
        add_syslinks("pthread")
    end
