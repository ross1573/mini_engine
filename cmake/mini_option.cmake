if (MSVC)
    add_compile_options(/Wall)
    add_compile_options(/external:W0)
    add_compile_options(/external:anglebrackets)
    add_compile_options(/Zc:preprocessor)
    add_compile_options(/MP)
    add_compile_options(/sdl)

    add_compile_options(/wd4061) # each case in switch is not handled explicitly
    add_compile_options(/wd4201) # nameless struct/union
    add_compile_options(/wd4251) # dll-interface missing (even in template)
    add_compile_options(/wd4265) # windows api stuff
    add_compile_options(/wd4365) # signed mismatch on explicit conversion
    add_compile_options(/wd4514) # unreferenced inline function has been removed
    add_compile_options(/wd4625) # copy constructor implicitly deleted
    add_compile_options(/wd4626) # operator implicitly deleted
    add_compile_options(/wd4702) # unreachable code (from if constexpr)
    add_compile_options(/wd4710) # function not inlined
    add_compile_options(/wd4711) # selected for automatic inline
    add_compile_options(/wd4820) # implicit padding
    add_compile_options(/wd5030) # unrecognized attribute
    add_compile_options(/wd5045) # spectre stuff
    add_compile_options(/wd5222) # unrecognized attribute
    add_compile_options(/wd5267) # implicit deprecation of special member functions
else()
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Wshadow)
    add_compile_options(-Wpedantic)
    add_compile_options(-Wconversion)
endif()