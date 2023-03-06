set_targetdir("dst")

target("uECC")
    set_kind("static")
    add_files("src/**.c")
    add_includedirs("inc", {public=true})

includes("command")
