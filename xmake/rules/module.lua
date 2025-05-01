if has_config("shipping_one_archive") then
    add_defines("SHIPPING_ONE_ARCHIVE")
end

rule("ai.component")
    on_config(function (component, opt)
        import("core.project.project")
        local owner_name = component:extraconf("rules", "ai.component", "owner")
        local owner = project.target(owner_name)
        local owner_api = owner:extraconf("rules", "ai.dyn_module", "api")
        -- insert owner's include dirs
        for _, owner_inc in pairs(owner:get("includedirs")) do
            component:add("includedirs", owner_inc, {public = true})
        end
        -- import api from owner
        if has_config("shipping_one_archive") then
            component:add("defines", owner_api.."_API=")
        else
            component:add("defines", owner_api.."_API=AI_IMPORT")
        end
    end)
rule_end()

rule("ai.dyn_module")
    add_deps("ai.module")
    on_load(function (target, opt)
        local api = target:extraconf("rules", "ai.dyn_module", "api")
        target:add("values", "ai.Attributes", "Module")
        target:add("defines", api.."_IMPL")
        target:add("defines", api.."_EXTERN_C=AI_EXTERN_C", {public=true})
        if has_config("shipping_one_archive") then
            target:add("defines","SHIPPING_ONE_ARCHIVE")
            target:add("defines", api.."_API=", {public=true})
        else
            target:add("defines", api.."_API=AI_IMPORT", {interface=true})
            target:add("defines", api.."_API=AI_EXPORT", {public=false})
        end
    end)
    on_config(function(target)
        if has_config("shipping_one_archive") then
            if target:kind() == "binary" then
                local output_dir = vformat("$(buildir)/$(os)/$(arch)/$(mode)")
                for _, dep in pairs(dep_modules) do
                    if is_plat("linux") then
                        target:add("ldflags", "-Wl,--whole-archive "..output_dir.."/lib"..dep..".a -Wl,--no-whole-archive", {force = true, public = false})
                    elseif is_plat("macosx") then
                        target:add("ldflags", "-Wl,-force_load "..output_dir.."/lib"..dep..".a", {force = true, public = false})
                    elseif is_plat("windows") then
                        target:add("ldflags", "/WHOLEARCHIVE:"..dep..".lib", {force = true, public = false})
                    end
                end
            end
        end
    end)
rule_end()
