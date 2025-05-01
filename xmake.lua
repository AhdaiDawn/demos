-- Set minimal xmake version
set_xmakever("2.9.9")

-- description scope extensions
includes("xmake/ai_desc_ext.lua")

-- global configs
ai_env_set("engine_dir", os.scriptdir())
default_unity_batch = 16

add_moduledirs("xmake/modules")
-- add_plugindirs("xmake/plugins")
add_repositories("ai-xrepo xrepo", {rootdir = os.scriptdir()})
includes("xmake/options.lua")
includes("xmake/compile_flags.lua")
includes("xmake/rules.lua")

-- global configs
ai_global_config()

-- load project configs
if not disable_project_config then
    if os.exists("project.lua") then
        includes("project.lua")
    else
        includes("xmake/project.default.lua")
    end
end

-- modules
includes("thirdparty")
includes("modules")

ai_includes_with_cull("tests")
ai_includes_with_cull("samples")
