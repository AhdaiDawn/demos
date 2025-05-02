-- imports
import("core.base.option")
import("core.base.hashset")
import("core.project.config")
import("core.project.project")
import("lib.detect.find_tool")
import("ai.utils")

local default_paths = {"modules", "tests", "samples"}
local exts = {"cpp", "c", "h", "hpp", "inc"}

function main()
    -- load configuration
    config.load()

    local clang_format = utils.find_program_in_system("clang-format")
    if not clang_format then
        raise("Didn't find clang-format")
    end

    local fd = utils.find_program_in_system("fd")
    if not fd then
        raise("Didn't find fd")
    end

    -- get options
    local opt_paths = option.get("paths")
    local opt_all = option.get("all")

    local paths = opt_paths

    if opt_all then
        paths = table.join(default_paths, opt_paths)
    end

    if not paths then
        raise("The path can't be empty!!!")
    end

    local ext_cmd = ""
    for _, ext in ipairs(exts) do
        ext_cmd = ext_cmd .. " -e " .. ext
    end

    local path_cmd = ""
    for _, path in ipairs(paths) do
        path_cmd = path_cmd .. " " .. path
    end

    local cmd_fd = format("fd %s . %s", ext_cmd, path_cmd)
    local cmd_format = format("fd %s . %s -x clang-format -i", ext_cmd, path_cmd)

    os.exec(cmd_fd)

    print(cmd_format)

    os.exec(cmd_format)

    print("format completed.")
end
