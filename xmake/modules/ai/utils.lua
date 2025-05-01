import("lib.detect")
import("core.project.config")
import("core.base.object")
import("core.base.option")
import("core.project.project")

------------------------dirs------------------------
function ai_build_artifact_dir()
    return "build/.ai/"
end
function ai_codegen_dir(target_name)
    return path.join(ai_build_artifact_dir(), "codegen", target_name)
end
function binary_dir()
    return vformat("$(buildir)/$(os)/$(arch)/$(mode)")
end
function download_dir()
    return path.join(ai_build_artifact_dir(), "download")
end
function log_file(sub_path)
    return path.join(ai_build_artifact_dir(), "log", sub_path..".log")
end

------------------------package name rule------------------------
function package_name_tool(tool_name)
    return vformat("%s-$(host)-%s.zip", tool_name, os.arch())
end
function package_name_sdk(sdk_name, opt)
    opt = opt or {}

    -- TODO. $(host) -> $(os)
    if opt.debug then
        return vformat("%s_d-$(host)-%s.zip", sdk_name, os.arch())
    else
        return vformat("%s-$(host)-%s.zip", sdk_name, os.arch())
    end
end

------------------------find------------------------
function find_program_in_paths(program_name, paths, opt)
    opt = opt or {}

    -- bad case
    if path.is_absolute(program_name) then
        raise("program name cannot be an absolute path")
    end

    -- find program
    for _, _path in ipairs(paths) do
        -- format path
        _path = path.join(vformat(_path), program_name)

        -- check path
        if os.isexec(_path) then
            return _path
        end
    end
end
function find_program_in_system(program_name, opt)
    opt = opt or {}

    -- load opt
    local use_which = opt.use_sys_find or true;
    local use_env_find = opt.use_env_find or true;

    -- helpers
    local _format_program_name_for_winos = function (name)
        local name = program_name:lower()
        if not name:endswith(".exe") then
            name = name .. ".exe"
        end
        return name
    end
    local _check_program_path = function (path)
        if path then
            path = path:trim()
            if os.isexec(path) then
                return path
            end
        end
    end

    -- find use sys find
    if use_which then
        if os.host() == "windows" then -- use where
            -- format program name
            local find_name = _format_program_name_for_winos(program_name)

            -- find
            local found_paths = nil
            try {
                function ()
                    found_paths = os.iorunv("where.exe", {find_name})
                end
            }
            if found_paths then
                for _, path in ipairs(found_paths:split("\n")) do
                    path = path:trim()
                    if #path > 0 then
                        if os.isexec(path) then
                            return path
                        end
                    end
                end
            end

        else -- use which
            local program_path = nil
            try {
                function ()
                    program_path = os.iorunv("which", {program_name})
                end
            }
            program_path = _check_program_path(program_path)
            if program_path then
                return program_path
            end
        end
    end

    -- find env find
    if use_env_find then
        if os.host() == "windows" then -- use registry
            -- local find_name = _format_program_name_for_winos(program_name)
            -- local program_path = winos.registry_query("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" .. find_name)
            -- program_path = _check_program_path(program_path)
            -- if program_path then
            --     return program_path
            -- end
        else -- use sys default path
            local paths = {
                "/usr/local/bin",
                "/usr/bin",
            }
            local path = find_program_in_paths(program_name, paths, opt)
            if path then
                return path
            end
        end
    end
end
function find_download_file(file_name)
    return detect.find_file(file_name, download_dir())
end
function find_download_package(package_name)
    return find_download_file(package_name_sdk(package_name, opt))
end
function find_download_package_tool(tool_name)
    return find_download_file(package_name_tool(tool_name))
end
function find_download_package_sdk(sdk_name, opt)
    return find_download_file(package_name_sdk(sdk_name, opt))
end
function find_tool(tool_name, sub_paths, opt)
    -- combine paths
    local paths = { install_dir_tool() }
    for _, sub_path in ipairs(sub_paths) do
        table.insert(paths, path.join(install_dir_tool(), sub_path))
    end

    -- find
    return find_program_in_paths(tool_name, paths, opt)
end

------------------------find spec program------------------------
function find_python()
    -- find embedded_python
    local embedded_python = find_tool("python", {"python-embed"})
    if embedded_python then
        return embedded_python
    end

    -- find system python
    local python_name = (os.host() == "windows") and "python" or "python3"
    local system_python = find_program_in_system(python_name)
    if system_python then
        return system_python
    end
end

------------------------lua utils------------------------
function setfenv(fn, env)
    local i = 1
    while true do
        local name = debug.getupvalue(fn, i)
        if name == "_ENV" then
        debug.upvaluejoin(fn, i, (function()
            return env
        end), 1)
        break
        elseif not name then
        break
        end

        i = i + 1
    end

    return fn
end
function getfenv(fn)
    local i = 1
    while true do
        local name, val = debug.getupvalue(fn, i)
        if name == "_ENV" then
            return val
        elseif not name then
            break
        end
        i = i + 1
    end
end
function redirect_fenv(fn, reference_fn)
    setfenv(fn, getfenv(reference_fn))
end
