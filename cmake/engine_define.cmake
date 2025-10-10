macro (engine_compile_definitions)
    list(APPEND engine_compile_definition ${ARGV} "\n")
endmacro()

engine_compile_definitions(
    "ENGINE_PROJECT_NAME=\"${ENGINE_PROJECT_NAME}\""
    "ENGINE_PROJECT_VERSION=\"${ENGINE_PROJECT_VERSION}\""
    "\n"
    PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,true,false>
    PLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,true,false>
    PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,true,false>
    "\n"
    "DEBUG=$<IF:$<CONFIG:Debug>,true,false>"
    "LIB_PREFIX=\"${BUILD_MODULE_PREFIX}\""
)

if (WIN32)
    # msvc shit needs to be applied ethier vs or clang
    engine_compile_definitions(
        "_ALLOW_KEYWORD_MACROS"
        "no_unique_address=msvc::no_unique_address"
    )
endif()

# define compiler specific compile definitions
# compiler generator expression aren't supported on file generate, 
# so they have to be handled manually
if (CMAKE_CXX_COMPILER_ID MATCHES AppleClang)
    set(APPLE_CLANG 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CLANG 1)
    engine_compile_definitions(
        "force_inline=clang::always_inline"
        "no_inline=clang::noinline"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNUC)
    set(GNUC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(MSVC 1)
    engine_compile_definitions(
        "force_inline=msvc::forceinline"
        "no_inline=msvc::noinline"
    )
else()
    message(FATAL_ERROR "unsupproted compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

# handle compiler specific definition here
engine_compile_definitions(
    CLANG=$<IF:$<BOOL:${CLANG}>,true,false>
    GNUC=$<IF:$<BOOL:${GNUC}>,true,false>
    MSVC=$<IF:$<BOOL:${MSVC}>,true,false>
    APPLE_CLANG=$<IF:$<BOOL:${APPLE_CLANG}>,true,false>
)

foreach (definition ${engine_compile_definition})
    # special case on new line
    if (definition STREQUAL "\n")
        string(APPEND parsed_list "\n")
        continue()
    endif()

    # parse string by assign operator
    string(FIND ${definition} "=" index)
    if (index GREATER 0)
        math(EXPR val_start "${index} + 1")
        math(EXPR padding_len "41 - ${index}")

        # add some padding
        if (padding_len LESS 0)
            set(padding_len 0)
        endif()
        string(REPEAT " " ${padding_len} padding)
    else()
        string(LENGTH ${definition} len)
        set(padding "")
        set(val_start ${len})
    endif()

    # split and concat arguments
    string(SUBSTRING ${definition} 0 ${index} def)
    string(SUBSTRING ${definition} "${val_start}" -1 val)
    string(CONCAT parsed "#define " ${def} ${padding} ${val} "\n")

    string(APPEND parsed_list ${parsed})
endforeach()

set(engine_define_header "${CMAKE_BINARY_DIR}/engine/define.generated.h")
set_property(GLOBAL PROPERTY ENGINE_DEFINE_HEADER ${engine_define_header})

file(GENERATE
    OUTPUT "${engine_define_header}"
    CONTENT "${parsed_list}"
    TARGET "${ENGINE_PROJECT_NAME}"
    FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
    NEWLINE_STYLE LF
)