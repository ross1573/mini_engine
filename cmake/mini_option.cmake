if (MSVC)
    add_compile_options(
        /Wall
        /external:W0
        /external:anglebrackets
        /Zc:preprocessor
        /MP
        /sdl

        /wd4061 # each case in switch is not handled explicitly
        /wd4201 # nameless struct/union
        /wd4251 # dll-interface missing (even in template)
        /wd4265 # windows api stuff
        /wd4365 # signed mismatch on explicit conversion
        /wd4514 # unreferenced inline function has been removed
        /wd4625 # copy constructor implicitly deleted
        /wd4626 # operator implicitly deleted
        /wd4702 # unreachable code (from if constexpr)
        /wd4710 # function not inlined
        /wd4711 # selected for automatic inline
        /wd4820 # implicit padding
        /wd5030 # unrecognized attribute
        /wd5045 # spectre stuff
        /wd5222 # unrecognized attribute
        /wd5267 # implicit deprecation of special member functions
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
    )
endif()