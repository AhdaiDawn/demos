-- 3.12.0
target("nlohmann_json")
    set_group("01.libraries")
    set_kind("headeronly")
    add_includedirs("./", {public=true})
