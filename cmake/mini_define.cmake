macro (add_module_definitions)
    list(APPEND COMPILE_DEFINITIONS ${ARGV} "\n")
endmacro()

add_module_definitions(
    ENGINE_PROJECT_NAME="${ENGINE_PROJECT_NAME}"
    ENGINE_PROJECT_VERSION="${ENGINE_PROJECT_VERSION}"
    "\n"
    PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,true,false>
    PLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,true,false>
    PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,true,false>
    "\n"
    DEBUG=$<IF:$<CONFIG:Debug>,true,false>
    LIB_PREFIX="${BUILD_MODULE_PREFIX}"
)

if (WIN32)
    # msvc shit needs to be applied ethier vs or clang
    add_module_definitions(
        _ALLOW_KEYWORD_MACROS
        no_unique_address=msvc::no_unique_address
    )
endif()

# define compiler specific compile definitions
# compiler generator expression aren't supported on file generate, 
# so they have to be handled manually
if (CMAKE_CXX_COMPILER_ID MATCHES AppleClang)
    set(APPLE_CLANG 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    set(CLANG 1)
    add_module_definitions(
        force_inline=clang::always_inline
        no_inline=clang::noinline
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNUC)
    set(GNUC 1)
elseif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    set(MSVC 1)
    add_module_definitions(
        force_inline=msvc::forceinline
        no_inline=msvc::noinline
    )
else()
    message(FATAL_ERROR "unsupproted compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

# handle compiler specific definition here
add_module_definitions(
    CLANG=$<IF:$<BOOL:${CLANG}>,true,false>
    GNUC=$<IF:$<BOOL:${GNUC}>,true,false>
    MSVC=$<IF:$<BOOL:${MSVC}>,true,false>
    APPLE_CLANG=$<IF:$<BOOL:${APPLE_CLANG}>,true,false>
)