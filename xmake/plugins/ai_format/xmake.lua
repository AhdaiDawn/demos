task("ai_format")
    set_category("plugin")
    on_run("main")
    set_menu {
        usage = "xmake ai_format [options] [paths]",
        description = "Format code by paths.(depend on fd & clang-format)",
        options = {
            -- 缩写，选项，类型[k|kv|vs]，默认值，描述，选项...
            {'a', "all",     "k",   nil,    "Format default paths."},
            {nil, "paths",   "vs",  nil,    "The paths list."}
        }
    }
