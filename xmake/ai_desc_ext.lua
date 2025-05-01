---------------------------------- global ----------------------------------
function ai_global_target()
    target("AI.Global")
end

function ai_global_target_end()
    target_end()
end

function ai_global_config()
    -- policy
    set_policy("build.ccache", false)
    set_policy("build.warning", true)
    set_policy("check.auto_ignore_flags", false)

    -- cxx options
    set_warnings("all")
    set_languages(get_config("cxx_version"), get_config("c_version"))
    add_rules("mode.debug", "mode.release", "mode.releasedbg", "mode.asan")

    -- cxx defines
    if (is_os("windows")) then
        add_defines("UNICODE", "NOMINMAX", "_WINDOWS")
        add_defines("_GAMING_DESKTOP")
        add_defines("_CRT_SECURE_NO_WARNINGS")
        add_defines("_ENABLE_EXTENDED_ALIGNED_STORAGE")
        add_defines("_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR") -- for preventing std::mutex crash when lock
        if (is_mode("release")) then
            set_runtimes("MD")
        elseif (is_mode("asan")) then
            add_defines("_DISABLE_VECTOR_ANNOTATION")
        else
            -- set_runtimes("MDd")
            set_runtimes("MD")
        end
    end
end

---------------------------------- env ----------------------------------
_ai_env_table = _ai_env_table or {}
_ai_env_committed = false

-- set env
function ai_env_set(key, value)
    _ai_env_table[key] = value
end

-- add env (trait value as list)
function ai_env_add(key, value)
    -- collect old value
    local old_value = _ai_env_table[key]
    if not old_value then
        old_value = {}
    elseif type(old_value) ~= "table" then
        old_value = { old_value }
    end

    -- insert new value
    table.insert(old_value, value)

    -- setup value
    _ai_env_table[key] = old_value
end

-- get env
function ai_env_get(key)
    return _ai_env_table[key]
end

---------------------------------- module ----------------------------------
function static_component(name, owner, opt)
    target(owner)
        add_deps(name, { public = opt and opt.public or true })
    target_end()

    target(name)
        set_kind("static")
        set_group("01.modules/"..owner.."/components")
        add_rules("ai.component", { owner = owner })
end

function shared_module(name, api, opt)
    target(name)
        set_group("01.modules/"..name)
        add_rules("ai.dyn_module", { api = api })
        if has_config("shipping_one_archive") then
            set_kind("static")
        else
            set_kind("shared")
        end
        on_load(function (target, opt)
            if has_config("shipping_one_archive") then
                for _, dep in pairs(target:get("links")) do
                    target:add("links", dep, {public = true})
                end
            end
        end)
        opt = opt or {}
        if opt.exception and not opt.noexception then
            set_exceptions("cxx")
        else
            set_exceptions("no-cxx")
        end
end

function executable_module(name, api, opt)
    target(name)
        set_kind("binary")
        add_rules("ai.dyn_module", { api = api })
        local opt = opt or {}
        if opt.exception and not opt.noexception then
            set_exceptions("cxx")
        else
            set_exceptions("no-cxx")
        end
end

function public_dependency(dep, setting)
    add_deps(dep, {public = true})
end

---------------------------------- cull & tagged disable ----------------------------------
function ai_cull(name)
    ai_env_add("culled_modules", name)
end

function ai_includes_with_cull(name, ...)
    local args={...}
    local culled = ai_env_get("culled_modules")
    ai_env_add("modules_with_cull", name)
    if (not culled) or (not table.contains(culled, name)) then
        if #args == 0 then
            includes(name)
        else
            includes(args)
        end
    end
end
---------------------------------- unity build ----------------------------------
function ai_unity_build()
    add_rules("c++.unity_build", {batchsize = default_unity_batch})
end
