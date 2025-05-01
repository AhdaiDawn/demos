target("quill")
    set_kind("headeronly")
    add_includedirs("quill", {public=true})
    add_headerfiles("quill/(**.h)",{prefixdir = "quill"})
