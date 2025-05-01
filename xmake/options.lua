option("cxx_version")
    -- "cxx20", "cxx17", "cxx23"
    set_default("cxx20")
    set_values("cxx20", "cxx17", "cxx23")
    set_description("c++ version of project")
option_end()

option("c_version")
    -- "c11"
    set_default("c11")
    set_values("c11")
    set_description("c version of project")
option_end()

option("use_profile")
    -- "auto", "enable", "disable"
    set_default("auto")
    set_values("auto", "enable", "disable")
    set_description("Toggle to use tracy profile")
option_end()

option("shipping_one_archive")
    set_default(false)
    set_showmenu(true)
    set_description("Toggle to build modules in one executable file.")
option_end()
